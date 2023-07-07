import argparse
import logging
import os
import subprocess
import textwrap

from luaparser import ast
from luaparser.astnodes import (Assign, If, LocalAssign, Index, Function, Call, String, Name, IndexNotation, Method, Invoke, Block, SetTabValue, Table, Node, Comment,
        AnonymousFunction, FunctionReference, ArrayIndex, Number, NumberType, Type, IAssign, InplaceOp, IAddTab, ISubTab, IMulTab, IDivTab,
        AndLoOp, OrLoOp, ULNotOp, Field, StringRef, StringDecl, TrueExpr, FalseExpr,
        )

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger()

"""
# TODO
## Syntax
print(3..a) dead
"""
def const_strings(tree):
    """
    Replaces all literal strings ("string_1") with StringRef("string_1")
    Also Table fields ({"a"=5})
    And Index access by name (a.b)
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)
    for n in tree_visitor.nodes:
        if not isinstance(n, ((String, Index, Field))):
            continue
        if isinstance(n, String):
            assert n.parent is not None, f"{n} ({n.s}) has no parent"
            n.parent.replace_child(n, StringRef(n.s))
        elif isinstance(n, Index) and n.notation == IndexNotation.DOT:
            if isinstance(n.idx, StringRef):
                # already replaced this.. problematic?
                continue
            # value.idx 
            assert isinstance(n.idx, Name)
            assert n.parent is not None, f"{n} ({n.dump()}) has no parent"
            n.idx = StringRef(n.idx.id, parent=n)
        elif isinstance(n, Field):
            ""
            #assert n.parent is not None, f"{n} has no parent"
            #n.parent.replace_child(n, Name(f'__str_{n.s.replace(" ", "_")}'))

def add_decls(tree):
    """
    Adds variable declarations in corresponding blocks. Globals are declared at the root chunk.
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = {}
    for n in tree_visitor.nodes:
        if not isinstance(n, (Assign, LocalAssign)):
            continue
        if isinstance(n, LocalAssign):
            for t in n.targets:
                if isinstance(t, Index):
                    continue
                key = id(t.scope())
                seen.setdefault(key, [])
                if t.id not in [a.id for _, a, _ in seen[key]]:
                    # True = local-assign
                    seen[key].append((t.scope(), t, True))
        elif isinstance(n, Assign):
            # global variables, always go to the root
            key = id(tree.body)
            for t in n.targets:
                if isinstance(t, Index):
                    continue
                local_key = id(t.scope())
                if t.id in [a.id for _, a, _ in seen.get(local_key, [])]:
                    # a LocalAssign (local var = ..) has been seen in the same scope
                    # as this variable; do not create a global
                    continue

                seen.setdefault(key, [])
                if t.id not in [a.id for _, a, _ in seen[key]]:
                    # False = non-local-assign
                    seen[key].append((tree.body, t, False))

    for names in seen.values():
        for block, name, is_local in names:
            block.add_declaration(name, is_local)

def rename_stdlib_calls(tree):
    """
    Rewrites certain function calls to not use C stdlib names, like

    `sqr` -> `_sqr`
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not isinstance(n, Call):
            continue
        if not isinstance(n.func, Name):
            continue
        if n.func.id in ['sqr', 'sqrt', 'ceil']:
            n.func.id = f'_{n.func.id}'

def move_to_preinit(tree):
    """
    Moves all top-level expressions to a custom __preinit() function.

    Top-level functions are excluded from this.
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)


    moved_nodes = []

    for n in tree_visitor.nodes:
        if n.parent != tree.body:
            continue
        if isinstance(n, Function):
            continue
        tree.body.body.remove(n)
        moved_nodes.append(n)
    _preinit = Function(Name("__preinit"), [], Block(moved_nodes), parent=tree.body)
    tree.body.body.append(_preinit)

def add_string_decls(tree):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    strings_to_decl = []

    __preinit = None
    for n in tree_visitor.nodes:
        if isinstance(n, Function) and n.name.id == "__preinit":
            __preinit = n
            continue
        if not isinstance(n, StringRef):
            continue
        if n.name not in [_n.name for _n in strings_to_decl]:
            strings_to_decl.append(n)
    assert __preinit is not None

    i = 0
    _seen_varname_by_str = {}
    for s in strings_to_decl[::-1]:
        # if string "a b c" and "a-b-c" exist, this should die
        assert _seen_varname_by_str.get(s.varname) in (None, s.name), f"Varname '{s.varname}' was seen as '{_seen_varname_by_str.get(s.varname)}' but is also '{s.name}'"
        sd = Assign([Name(s.varname)], [StringDecl(s.name, i)], parent=__preinit)
        __preinit.body.body.insert(0, sd)
        i += 1

    __preinit.body.body.insert(0, Call(Name("_grow_strings_to"), [Number(str(i), ntype=NumberType.BARE_INT)], parent=__preinit))


def _find_index_at_closest_block(t: Node) -> tuple[int, Block]:
    if t.parent is None:
        raise ValueError("Did not find a block while going up")
    if isinstance(t.parent, Block):
        if t in t.parent.body:
            return t.parent.body.index(t), t.parent
    return _find_index_at_closest_block(t.parent)

def _first_parent_of_type(t: Node, valid: tuple):
    if t.parent is None:
        return None
    if isinstance(t.parent, valid):
        return t.parent
    return _first_parent_of_type(t.parent, valid)

def transform_anonymous_tables(tree):
    """
    Creates local variables for anonymous tables
    ```
    func({a=1})
    ```
    =>
    ```
    __anonymous_table_0 = {a=1}
    func(__anonymous_table_0)
    ```

    Which also flattens nested tables:
    ```
    a = {x=5,
         y={5=7},
        }
    ```

    =>
    ```
    __a_subtable_y = {5=7}
    a = {x=5, y=__a_subtable_y}
    ```

    """

    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    anon_table_count = 0
    for n in tree_visitor.nodes:
        if not isinstance(n, Table):
            continue
        if not n.parent:
            continue
        if isinstance(n.parent, Assign):
            # Regular table creation
            continue

        _pt = _first_parent_of_type(n, (Table, Index))  # TODO Call Block ?
        if _pt is None:
            continue

        _name_prefix = "idk_good_name_prefix"
        if isinstance(_pt, Assign):
            assert _pt not in _pt.targets, "a Table can never be an assignment target"
            assert len(_pt.targets) == 1, "Sad, can't handle multi values"
            _name_prefix = _pt.targets[0].id

        _name_prefix = f'__subtable_{_name_prefix}_'

        _key = "idk_key"
        idx, _closest_block = _find_index_at_closest_block(_pt)
        table_to_replace = None
        if isinstance(_pt, Table):
            # On nested tables, the value must be replaced
            for f in _pt.fields:
                if n != f.value:
                    continue
                if isinstance(f.key, String):
                    _key = _name_prefix + f.key.s
                table_to_replace = f.value
                f.value = Name(_key)
                break
        elif isinstance(_pt, Index):
            if _pt.value == n:  # Anonymous table, being indexed by _pt.idx
                table_to_replace = _pt.value
                _key = f"_anonymous_table_{anon_table_count}"
                _pt.value = Name(_key)
                anon_table_count += 1

        _closest_block.body.insert(idx, LocalAssign([Name(_key)], [table_to_replace], parent=_closest_block,
                                                    first_token=n.first_token, last_token=n.last_token,
                                                    ))


def transform_literal_tables_to_assignments(tree):
    """
    Rewrite literal tables `a = {x=5, y=6}` into
    ```
    a = {}
    SetTabValue(a, "x", 5)
    SetTabValue(a, "y", 6)
    ```
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not isinstance(n, Table):
            continue
        if not n.parent:
            assert False
        if not isinstance(n.parent, Assign):
            continue
        _assign = n.parent
        if n not in _assign.values:
            continue
        assert len(_assign.targets) == 1
        assert len(_assign.values) == 1
        # some day

        # assuming Assign can only happen in Block
        assert isinstance(_assign.parent, Block)

        _assign_idx = _assign.parent.body.index(_assign)
        _target_table = _assign.targets[0]
        if isinstance(_target_table, Index):
            """
            this function (transform_literal_tables_to_assignments) does not _replace_ the existing node,
            instead it _adds_ a new node.

            if isinstance(_assign.targets[0], Index),
            it is `a.b.c = 1`, which should be serialized as `set_tabvalue(get_tabvalue(..), key, value)`

            Mutating the original reference node (the get_tabvalue, Index) is problematic, as that node's `parent`
            attribute will get out of sync, when the AST looks like

            Assign
                Index (gettabvalue)
            SetTabValue
                Index (gettabvalue)

            when calling `SetTabValue`, it sets the `parent` attribute on all the passed arguments to itself
            so instead we pass `SetTabValue` a copy of the original node

            """
            _target_table = Index(_target_table.idx, _target_table.value, _target_table.notation)
        for f in n.fields[::-1]:
            key = f.key
            if isinstance(key, Name):
                key = String(key.id)
            settabvalue = SetTabValue(_target_table, key, f.value, parent=_assign.parent,
                                      first_token=n.first_token, last_token=n.last_token)
            _assign.parent.body.insert(_assign_idx+1, settabvalue)
        if len(n.fields) > 0:
            _assign.parent.body.insert(_assign_idx+1, Comment(f"Fields for table {_assign.targets[0].id}", parent=_assign.parent))



def ensure_table_fields(tree):
    """
    Ensure that every Table is created with sufficient capacity for known keys
    ```
    a = {}
    a.b = 5
    ```
    =>
    ```
    a = make_table(1) // 1 as field b is known statically
    ```
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not isinstance(n, SetTabValue):
            continue
        assert n.parent is not None
        for s in n.parent.body:
            if not isinstance(s, Assign):
                continue
            if n.table.id not in [t.id for t in s.targets]:
                continue
            assert len(s.targets) == 1
            if not isinstance(s.values[0], Table):
                # TODO ?? what's ArrayIndex doing
                continue
            d = n.key.dump()
            if d not in s.values[0].field_names:
                s.values[0].field_names.append(d)

def __flattened_children(node, state):
    for c in node.children():
        state.append(c)
        __flattened_children(c, state)
    return state

def _flattened_children(node):
    return __flattened_children(node, [])

def transform_logical_operators(tree):
    """
    Rewrite logical operators into nested branches:

    ```
    a = false and func(13)
    ```
    into
    ```
    let and_result = false
    if and_result then
        and_result = func(13)
    end
    ```

    LoOp can be nested arbitrarily deep within a block, as an example:

                                       |    Block
                                       |      Assign
                                       |        Target
                                       |        Value
                 a = (b or c) * d      |          MultOp
                                       |            OrLoOp
                                       |              Name
                                       |              Name
                                       |            Name


    we need to insert the temp var within the _block_, right before
    the first-parent-that-is-a-direct-child-of-the-block (in the example, before Assign)
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)
    i = 0
    j = 0

    def _is_simple_lo_op(node):
        simple_types = (Name, Number, TrueExpr, FalseExpr)
        return isinstance(node.left, simple_types) and isinstance(node.right, simple_types)

    had_nested = False
    for n in tree_visitor.nodes:
        if not isinstance(n, (AndLoOp, OrLoOp)):
            continue
        assert n.parent

        if _is_simple_lo_op(n):
            continue

        # if it's a nested condition, come back later
        lo_ops = [c for c in _flattened_children(n) if isinstance(c, (AndLoOp, OrLoOp,))]
        if any([not _is_simple_lo_op(c) for c in lo_ops]):
            had_nested = True
            continue

        parent_block = _first_parent_of_type(n, (Block,))
        assert parent_block is not None, f'Could not find a parent Block for {n} {n.dump()}'
        direct_child_of_parent_block = n
        while direct_child_of_parent_block.parent != parent_block:
            assert direct_child_of_parent_block.parent is not None
            direct_child_of_parent_block = direct_child_of_parent_block.parent

        idx = parent_block.body.index(direct_child_of_parent_block)

        if isinstance(n, (AndLoOp)):
            """
            (a and b)

            If 'a' is true: return 'b'
            return 'a'

            ->

            __tmp_and_var = a
            if (__tmp_and_var) then
              __tmp_and_var = b
            end

            """
            # Using _varname instead of creating a `Name` here as
            # on creation of the other nodes (Assign, LocalAssign, If), they will
            # try and set `parent` on it to themselves. Each should get its own instance
            # of `Name`.
            _varname = f"__tmp_and_var_{i}"
            i += 1
            temp = LocalAssign([Name(_varname)], [n.left], parent=parent_block)

            _ifbody = Assign([Name(_varname)], [n.right], parent=n.parent)
            _if = If(Name(_varname), Block([_ifbody]), Block([]), parent=parent_block)

            parent_block.body.insert(idx, _if)
            parent_block.body.insert(idx, temp)

            n.parent.replace_child(n, Name(_varname))
        elif isinstance(n, (OrLoOp)):
            _varname = f"__tmp_or_var_{j}"
            j += 1
            temp = LocalAssign([Name(_varname)], [n.left], parent=parent_block)
            _ifbody = Assign([Name(_varname)], [n.right], parent=n.parent)
            _if = If(ULNotOp(Name(_varname)), Block([_ifbody]), Block([]), parent=parent_block)

            parent_block.body.insert(idx, _if)
            parent_block.body.insert(idx, temp)

            n.parent.replace_child(n, Name(_varname))

    if had_nested:
        transform_logical_operators(tree)

def transform_index_assign(tree):
    """
    Rewrite Index-Assign `a.b = 5` into `SetTabValue(a, "b", 5)`
    Rewrite Index-IAssign `a.b += 5` into `AddTabValue(a, "b", 5)`
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    # FIXME these show the same Assign twice
    for n in tree_visitor.nodes:
        if not isinstance(n, Index):
            continue
        assert n.parent is not None, f'{n} - {n.value.id}.{n.idx.id}'

        if not isinstance(n.parent, (IAssign, Assign)):
            continue

        if isinstance(n.parent, Assign):
            _assign = n.parent
            if n not in _assign.targets:
                # var = tab.idx
                continue
            assert len(_assign.targets) == len(_assign.values)
        elif isinstance(n.parent, IAssign):
            _assign = n.parent
            if n != _assign.target:
                continue
        else:
            assert False, "Only Assign and IAssign supported"

        if not _assign in _assign.parent.body:
            # FIXME, why is this iterating over the same Assign twice?
            #assert False
            continue
        _key = n.idx
        if n.notation == IndexNotation.DOT:
            # DOT notation (a.b) is always a string (a["b"])
            _key = String(n.idx.id)
        else:
            if isinstance(n.idx, String):
                # bracket notation could either be a string (a["b"])
                _key = n.idx
            else:
                # or a name (a[var]) or number (a[5])
                _key = n.idx
        assert _key
        if isinstance(n.parent, Assign):
            _this_idx = _assign.targets.index(n)
            # Do not want the original values mutated
            new_assign_elem = SetTabValue(n.value.copy(), _key, _assign.values[_this_idx].copy(), first_token=n.first_token, last_token=n.last_token)

            if len(_assign.targets) == 1:
                _assign.parent.replace_child(_assign, new_assign_elem)
            else:
                # when working on a.key, b.key2 = 1, 2
                # pop the target and value, and prepend the Assign with a SetTabValue

                # insert first, as otherwise `assign` won't pass equality checks
                _assign.parent.prepend_child(_assign, new_assign_elem)

                _assign.targets.pop(_this_idx)
                _assign.values.pop(_this_idx)

        if isinstance(n.parent, IAssign):
            _map = {InplaceOp.ADD: IAddTab,
                    InplaceOp.SUB: ISubTab,
                    InplaceOp.MUL: IMulTab,
                    InplaceOp.DIV: IDivTab,
                    }
            new_assign_elem = _map[n.parent.op](n.value, _key, _assign.value, first_token=n.first_token, last_token=n.last_token)
            _assign.parent.replace_child(_assign, new_assign_elem)


def transform_table_functions(tree):
    """
    Rewrite table-based functions (`function tab.fn() ... end`) into:
    ```
    function __table_function_tab_fn(...) ... end
    tab.fn = TFUN(__table_function_tab_fn)
    ```
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not isinstance(n, (Function)):
            continue
        if not n.parent:
            print("NO PARENT??")
            continue
        if not isinstance(n.name, Index):
            continue

        _callable_name = f"__table_func_{n.name.value.id}_{n.name.idx.id}"
        n.parent.replace_child(n, Assign([n.name], [FunctionReference(Name(_callable_name))], parent=n.parent))

        # extract the lambda to be a normal function
        tree.body.body.append(Function(Name(_callable_name), n.args, n.body, parent=tree.body))
        # TODO:
        # - Read/Write _enclosed_ variables from UpValue table
        #   - How to know when it's an UpValue ???

def _find_table_key(t: Table, value):
    for f in t.fields:
        if f.value == value:
            return f.key.id # assuming it is always Name
def transform_anonymous_functions(tree):
    """
    Rewrite AnonymousFunction (`a = function() ... end`) into:
    ```
    function __anonymous_function_a_1(...) ... end
    a = TFUN(__anonymous_function_a_1)
    ```
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    i = 0
    for n in tree_visitor.nodes:
        if not isinstance(n, (Function, AnonymousFunction)):
            continue
        if not n.parent:
            print("transform_anon_func NO PARENT??")
            continue
        if isinstance(n, Function): 
            if not _is_inside(n, Function) and not _is_inside(n, Table):
                continue
        if isinstance(n, AnonymousFunction):
            _callable_name = "__anonymous_function"
            if isinstance(n.parent, Assign):
                assert len(n.parent.targets) == 1
                _callable_name += f'_{n.parent.targets[0].id}'
            elif isinstance(n.parent, Table) and isinstance(n.parent.parent, Assign):
                assert len(n.parent.parent.targets) == 1
                _callable_name += f'_{n.parent.parent.targets[0].id}_{_find_table_key(n.parent, n)}'
            else:
                _callable_name = f'{_callable_name}_{i}'
                i += 1
            n.parent.replace_child(n, FunctionReference(Name(_callable_name)))
        else:
            _callable_name = f"__nested_func_{n.name.id}" # FIXME: should include parent's name
            n.parent.replace_child(n, Assign([n.name], [FunctionReference(Name(_callable_name))], parent=n.parent))

        # extract the lambda to be a normal function
        tree.body.body.append(Function(Name(_callable_name), n.args, n.body, parent=tree.body))
        # TODO:
        # - Read/Write _enclosed_ variables from UpValue table
        #   - How to know when it's an UpValue ???

def transform_functions_to_vec_args(tree):
    """
    Move function arguments to be read from a table argument
    ```
    function(a,b,c) ... end
    ```
    =>
    ```
    function __anonymous_function__table_key(Table_t* args)
      a = args[0]
      b = args[1]
      c = args[2]
      ...
    end
    ```
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    # this tree is processed twice?
    for n in tree_visitor.nodes:
        if not isinstance(n, Function):
            continue
        if n.name.id in ["main", "__main", "_update", "_draw"]:
            continue
        if n.name.id == "_init":
            n.name.id = "__init"  # _init clashes with an internal elf function
            continue
        # after args declaration, assign the index value
        for arg in n.args[::-1]:
            idx = n.args.index(arg)
            aidx = ArrayIndex(Number(idx, ntype=NumberType.BARE_INT), Name('function_arguments'), parent=n.body)
            n.body.body.insert(0, LocalAssign([arg], [aidx], parent=n.body))

        n.args = [Name('function_arguments', type=Type.SLICE, parent=n)]

def transform_methods(tree):
    """
    Rewrite methods (`function tab:method() ... end`) into:
    ```
    function __tab_method(...) ... end
    tab["method"] = __tab_method
    ```

    And method calls (`tab:method(arg)`) into table calls with a self-argument and an argument array:

    ```
    tab['method'](tab, {arg})
    ```

    Further passes will optimize this access into a fast-field access.
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not n.parent:
            continue
        if isinstance(n, Method):
            _args = [Name('self', type=Type.UNKNOWN)] + n.args
            replacement_f = Function(Name(f'__{n.source.id}_{n.name.id}'), _args, n.body, parent=tree.body)
            assign = SetTabValue(n.source, String(n.name.id), FunctionReference(replacement_f.name))
            assign.parent = n.parent

            n.parent.replace_child(n, assign)
            tree.body.body.append(replacement_f)  # extract the lambda to be a normal function
            # which must be at the root of the tree
        if isinstance(n, Invoke):
            replacement = n.replace_with_idx_call()
            n.parent.replace_child(n, replacement)
            #n.parent.replace_child(n, replacement)


def _is_inside(node, what):
    if node.parent is None:
        return False
    if isinstance(node.parent, what):
        return True
    return _is_inside(node.parent, what)

def add_signatures(tree):
    """
    Adds forward declaration for functions, on the root chunk
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not isinstance(n, Function):
            continue
        if _is_inside(n, Function):
            continue
        if n.name.id == 'main':
            n.name.id = '__main'
        tree.body.add_signatures(n)

def set_parent_on_children(tree):
    """
    When the tree is constructed top-down, some Nodes do not know
    who they will be until they parse their children

    Those children cannot be passed `parent`
    """

    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        n.set_parent_on_children()

def transform(src, pretty=True, dump_ast=False, testing_params=None):
    tree = ast.parse(src)
    set_parent_on_children(tree)

    rename_stdlib_calls(tree)
    transform_anonymous_functions(tree)
    transform_table_functions(tree)
    transform_methods(tree)
    transform_functions_to_vec_args(tree) # this transforms methods as well
    transform_anonymous_tables(tree)
    transform_literal_tables_to_assignments(tree)
    transform_index_assign(tree)
    transform_logical_operators(tree)
    ensure_table_fields(tree)
    # these are "post-processing" and should always be after
    # everything that appends nodes to the tree
    move_to_preinit(tree)
    add_signatures(tree) # should go after everything that creates functions, including move_to_preinit
    const_strings(tree)
    add_string_decls(tree) # depends on const_strings, preinit having run
    add_decls(tree)

    if dump_ast:
        print(ast.to_pretty_str(tree))


    gen = '''#include "pico8.h"
#include "lua.h"
    '''
    ret = gen
    ret += tree.body.dump()
    if testing_params != None:
        ret += textwrap.dedent('''
        int main() {
            __preinit();
            __main();
            return 0;
        }''')

    if pretty:
        ret = prettify(ret)
    return ret

def prettify(src):
    p = subprocess.Popen(['clang-format'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    out, err = p.communicate(input=src.encode(), timeout=1)
    return out.decode()


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--object-file", help="Output object file. Something like tennis_p8", required=False, default=None)
    parser.add_argument("source", help="Lua source code. Not a cart.")
    return parser.parse_args()

def main():
    args = parse_args()
    with open(args.source) as fd:
        src = fd.read()

    t = transform(src, pretty=args.object_file is None, dump_ast=False)
    if not args.object_file:
        print(t)
        return
    with open("tmp.c", "w") as fd:
        fd.write(t)
    subprocess.run(['gcc', 
                    '-I.', '-I/home/david/git/PicoPico/src',
                    '-fPIC', '-shared',
                    '-g', '-O2',
                    '-std=c11',
                    '-o', args.object_file,
                    'tmp.c',
                    'fix32.c', os.path.expanduser('~/git/PicoPico/src/pico8.c'), 'lua.c'])
    print(f"File at {args.object_file}")
if __name__ == '__main__':
    main()
