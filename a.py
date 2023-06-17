import logging
import subprocess
import sys
import textwrap

from luaparser import ast
from luaparser.astnodes import (Assign, LocalAssign, Index, Function, Call, String, Name, IndexNotation, Method, Invoke, Block, SetTabValue, Table, Node, Comment,
        AnonymousFunction, FunctionReference, ArrayIndex, Number, NumberType, Type, IAssign, InplaceOp, IAddTab,
        )

# TODO: broken parsing when declaring local variables with no value:
# ```
# local j
# ```

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger()

def free_local_tables(tree):
    """
    Call `free_table` for every LocalAssign
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)
    for n in tree_visitor.nodes:
        if not isinstance(n, LocalAssign):
            continue
        _assign_scope = _first_parent_of_type(n, (Block,))
        assert len(n.targets) == len(n.values)
        for i in range(0, len(n.targets)):
            target = n.targets[i]
            value = n.values[i]
            if not isinstance(value, Table):
                continue
            # TODO: This is len-1 as it includes the "free" return at the end of the function
            # what happens when there are multiple return paths though?
            _call = Call(Name("free_tvalue"), [target], parent=_assign_scope)
            _assign_scope.body.insert(len(_assign_scope.body)-1, _call)

def add_decls(tree):
    """
    Adds variable declarations in corresponding blocks. Globals are declared at the root chunk.
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = {}
    for n in tree_visitor.nodes:
        if isinstance(n, LocalAssign):
            for t in n.targets:
                if isinstance(t, Index):
                    continue
                key = id(t.scope())
                seen.setdefault(key, [])
                if t.id not in [a.id for _, a in seen[key]]:
                    seen[key].append((t.scope(), t))

        elif isinstance(n, Assign):
            # global variables, always go to the root
            key = id(tree.body)
            for t in n.targets:
                if isinstance(t, Index):
                    continue
                seen.setdefault(key, [])
                if t.id not in [a.id for _, a in seen[key]]:
                    seen[key].append((tree.body, t))

    for names in seen.values():
        for block, name in names:
            block.add_declaration(name)

def rename_stdlib_calls(tree):
    """
    Rewrites certain function calls to not use C stdlib names, like

    `sin` -> `fix32::sin`
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not isinstance(n, Call):
            continue
        if n.func.id in ['sin', 'cos']:
            n.func.id = f'fix32::{n.func.id}'
        if n.func.id in ['sqrt', 'atan2']:
            n.func.id = f'{n.func.id}f'

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

    _preinit = Function(Name("__preinit"), [], Block(moved_nodes))
    tree.body.body.append(_preinit)

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
            continue
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
        for f in n.fields[::-1]:
            key = f.key
            if isinstance(key, Name):
                key = String(key.id)
            settabvalue = SetTabValue(_assign.targets[0], key, f.value, parent=_assign.parent,
                                      first_token=n.first_token, last_token=n.last_token)
            _assign.parent.body.insert(_assign_idx+1, settabvalue)
        if len(n.fields) > 0:
            _assign.parent.body.insert(_assign_idx+1, Comment(f"Fields for table {_assign.targets[0].id}"))
        n.fields = []  # clear content of the table



def transform_index_assign(tree):
    """
    Rewrite Index-Assign `a.b = 5` into `SetTabValue(a, "b", 5)`
    Rewrite Index-IAssign `a.b += 5` into `AddTabValue(a, "b", 5)`
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not n.parent:
            continue
        if not isinstance(n, Index):
            continue
        if not isinstance(n.parent, (IAssign, Assign)):
            continue

        if isinstance(n.parent, Assign):
            _assign = n.parent
            if n not in _assign.targets:
                continue
            assert len(_assign.targets) == 1
            assert len(_assign.values) == 1
            # some day
        elif isinstance(n.parent, IAssign):
            _assign = n.parent
            if n != _assign.target:
                continue
        else:
            assert False, "Only Assign and IAssign supported"

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
            new_assign_elem = SetTabValue(n.value, _key, _assign.values[0], first_token=n.first_token, last_token=n.last_token)
        if isinstance(n.parent, IAssign):
            _map = {InplaceOp.ADD: IAddTab,
                    #InplaceOp.SUB: ISubTab,
                    #InplaceOp.MUL: IMulTab,
                    #InplaceOp.DIV: IDivTab,
                    }
            new_assign_elem = _map[n.parent.op](n.value, _key, _assign.value, first_token=n.first_token, last_token=n.last_token)
        _assign.parent.replace_child(_assign, new_assign_elem)

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

    for n in tree_visitor.nodes:
        if not isinstance(n, (Function, AnonymousFunction)):
            continue
        if not n.parent:
            print("NO PARENT??")
            continue
        if not _is_inside(n, Function):
            continue
        #import pudb
        #pudb.set_trace()
        if isinstance(n, AnonymousFunction):
            _callable_name = "__anonymous_function"
            if isinstance(n.parent, Assign):
                assert len(n.parent.targets) == 1
                _callable_name += f'_{n.parent.targets[0].id}'
            n.parent.replace_child(n, FunctionReference(Name(_callable_name)))
        else:
            _callable_name = f"__nested_func_{n.name.id}" # FIXME: should include parent's name
            n.parent.replace_child(n, Assign([n.name], [FunctionReference(Name(_callable_name))]))

        # extract the lambda to be a normal function
        tree.body.body.append(Function(Name(_callable_name), n.args, n.body))
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

    for n in tree_visitor.nodes:
        if not isinstance(n, Function):
            continue
        if n.name.id in ["main", "__main"]:
            continue
        # after args declaration, assign the index value
        for arg in n.args[::-1]:
            idx = n.args.index(arg)
            aidx = ArrayIndex(Number(idx, ntype=NumberType.BARE_INT), Name('function_arguments'))
            #n.body.body.insert(len(n.args), LocalAssign([arg], [aidx], parent=n.body))
            n.body.body.insert(0, LocalAssign([arg], [aidx], parent=n.body))

        n.args = [Name('function_arguments', type=Type.UNK_PTR)]

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
            replacement_f = Function(Name(f'__{n.source.id}_{n.name.id}'), _args, n.body)
            assign = SetTabValue(n.source, String(n.name.id), FunctionReference(replacement_f.name))
            assign.parent = n.parent

            n.parent.replace_child(n, assign)
            tree.body.body.append(replacement_f)  # extract the lambda to be a normal function
            # which must be at the root of the tree
        if isinstance(n, Invoke):
            replacement = n.replace_with_idx_call()
            n.parent.replace_child_multi(n, [replacement])
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

def find_static_table_accesses(tree):
    """
    Flags constant table accesses (tab["const"] or tab.const) as "optimizable" 

    Returns a sorted, deduplicated list of all constant field names.
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    values = set()
    for n in tree_visitor.nodes:
        if not isinstance(n, Index):
            continue
        if isinstance(n.idx, String):
            values.add(n.idx.s)
            n.optimized_access = True
        elif isinstance(n.idx, Name) and n.notation == IndexNotation.DOT:
            values.add(n.id)
            n.optimized_access = True
        else:
            logger.debug("Not optimizing non-string/constant value", getattr(n.idx, 'id', getattr(n.idx, 'n', n.id)))
    return sorted(values)

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

def transform(src, pretty=True, dump_ast=False, testing=False):
    tree = ast.parse(src)
    set_parent_on_children(tree)

    rename_stdlib_calls(tree)
    transform_anonymous_functions(tree)
    transform_methods(tree)
    transform_functions_to_vec_args(tree) # this transforms methods as well
    transform_anonymous_tables(tree)
    transform_literal_tables_to_assignments(tree)
    transform_index_assign(tree)
    move_to_preinit(tree)
    add_signatures(tree)
    add_decls(tree)
    free_local_tables(tree)
    #static_table_fields = find_static_table_accesses(tree)
    #static_table_fields = [] # FIXME LATER

    if dump_ast:
        print(ast.to_pretty_str(tree))

    #_field_to_const = {x: f'FIELD_{x.upper()}' for x in static_table_fields}
    #ff_len = len(static_table_fields)
    #var_init = '\n'.join([f'fields["{x}"]\t= &fast_fields[{_field_to_const[x]}];' for x in static_table_fields])
    #field_to_idx = '\n'.join(f'const uint16_t {_field_to_const[x]} = {i};' for i, x in enumerate(static_table_fields))
    #idx_to_name = f'const TValue* idx_to_name[{ff_len}] = {{' + ', '.join(f'new TValue("{var}")' for var in static_table_fields) + '};'


    #gen = string.Template('''#include "lua.c"
    #        ''').substitute(field_to_idx=field_to_idx, var_init=var_init, idx_to_name=idx_to_name, ff_len=ff_len)

    gen = '#include "lua.c"\n'
    ret = gen
    ret += tree.body.dump()
    if testing:
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


if __name__ == '__main__':
    with open(sys.argv[1]) as fd:
        src = fd.read()

    t = transform(src, pretty=True, dump_ast=False)
    print(t)
