import argparse
import logging
import subprocess
import textwrap

from luaparser import ast
from luaparser.astnodes import (Assign, If, LocalAssign, Index, Function, Call, String, Name, IndexNotation, Method, Invoke, Block, SetTabValue, Table, Node, Comment,
        AnonymousFunction, FunctionReference, ArrayIndex, Number, NumberType, Type, IAssign, InplaceOp, IAddTab, ISubTab, IMulTab, IDivTab,
        AndLoOp, OrLoOp, ULNotOp, Field, StringRef, StringDecl, TrueExpr, FalseExpr, Fornum, Return,
        )

from typing import Optional, TypeAlias

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger()

_Scope: TypeAlias = Function | AnonymousFunction | Method | Fornum
Scope = (Function, AnonymousFunction, Method, Fornum)
_FunctionLike: TypeAlias = Function | AnonymousFunction | Method
FunctionLike = (Function, AnonymousFunction, Method)

"""
# TODO

Initialize TFUN once; now that it's not just a pointer to function, it's kinda expensive

Function call optimization:
When a function is:
 - never stored in a table
 - always called with the same # of arguments
 - always match arg # at caller and definition

then it can be optimized to remove `CALL` _and_ to pass the arguments directly

remove `_bool()` on `if(_bool(_equal(...)))`

remove the indirection ? with lambda_args requiring a dict lookup? could be constant int+vector?
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
            assert isinstance(n.idx, Name), n.dump()
            assert n.parent is not None, f"{n} ({n.dump()}) has no parent"
            n.idx = StringRef(n.idx.id, parent=n)
        elif isinstance(n, Field):
            ""
            #assert n.parent is not None, f"{n} has no parent"
            #n.parent.replace_child(n, Name(f'__str_{n.s.replace(" ", "_")}'))

def all_parent_scopes(elem):
    ret = [elem.scope()]
    if elem.parent is not None:
        ret.extend(all_parent_scopes(elem.parent))
    return ret

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
                # Need to look for any local-assign on _every_ scope going up, as there could be an assign
                # in an If branch or similar sub-scope, for a variable defined in the same function
                _scopes = all_parent_scopes(t)
                _scope_ids = [id(s) for s in _scopes]
                _nested_vars = [seen.get(_scope_id, []) for _scope_id in _scope_ids]
                _vars_ids = [var.id for items in _nested_vars for _, var, _ in items]
                if t.id in _vars_ids:
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

    `sqrt` -> `_sqrt`
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    _stdlib_overlap = ['sqrt', 'ceil', 'sin', 'cos', 'atan2', 'abs', 'time', 'min', 'max']
    _custom_overlap = ['remove', 'y0']
    _to_rename = _stdlib_overlap + _custom_overlap
    for n in tree_visitor.nodes:
        if not isinstance(n, Name):
            continue
        if n.id not in _to_rename:
            continue
        n.id = f'_{n.id}'


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

        _pt = _first_parent_of_type(n, (Table, Index, Call, Field))
        if _pt is None:
            continue

        _name_prefix = "idk_good_name_prefix"
        if isinstance(_pt, Assign):
            assert _pt not in _pt.targets, "a Table can never be an assignment target"
            assert len(_pt.targets) == 1, "Sad, can't handle multi values"
            _name_prefix = _pt.targets[0].id

        _name_prefix = f'__subtable_{_name_prefix}_'

        _key = "idk_key"
        table_to_replace = None
        if isinstance(_pt, Field):
            if isinstance(_pt.key, String):
                _key = _name_prefix + _pt.key.s
            table_to_replace = _pt.value
            _pt.value = Name(_key)
        elif isinstance(_pt, Index):
            if _pt.value == n:  # Anonymous table, being indexed by _pt.idx
                table_to_replace = _pt.value
                _key = f"_anonymous_table_{anon_table_count}"
                _pt.value = Name(_key)
                anon_table_count += 1
        elif isinstance(_pt, Call):
            for arg in _pt.args:
                if arg == n:
                    table_to_replace = arg
                    _key = f"_anonymous_table_{anon_table_count}"
                    _pt.replace_child(arg, Name(_key))
                    anon_table_count += 1

        idx, _closest_block = _find_index_at_closest_block(_pt)
        _closest_block.body.insert(idx, LocalAssign([Name(_key)], [table_to_replace], parent=_closest_block,
                                                    first_token=n.first_token, last_token=n.last_token,
                                                    ))

def _literal_table_return(n):
    _return = n.parent
    assert isinstance(_return, Return)
    # assuming Assign can only happen in Block
    assert isinstance(_return.parent, Block)

    return_idx = _return.parent.body.index(_return)
    temp_var = Name("_anon_return_table")
    la = LocalAssign([temp_var], [n], parent=_return.parent)
    _return.parent.body.insert(return_idx, la)
    la_idx = _return.parent.body.index(la)
    for f in n.fields[::-1]:
        key = f.key
        if isinstance(key, Name) and not f.between_brackets:
            # a.x  -> SetTabValue(a, "x", val)
            # a[x] -> SetTabValue(a,   x, val)
            key = String(key.id)
        settabvalue = SetTabValue(temp_var, key, f.value, parent=_return.parent,
                                  first_token=n.first_token, last_token=n.last_token)
        _return.parent.body.insert(la_idx+1, settabvalue)
    _return.values = [temp_var]

def _literal_table_assign(n):
    _assign = n.parent
    if n not in _assign.values:
        return
    # TODO also allow asymmetric
    assert len(_assign.targets) == len(_assign.values)
    # some day

    # assuming Assign can only happen in Block
    assert isinstance(_assign.parent, Block)

    _table_value_idx = _assign.values.index(n)

    _assign_idx = _assign.parent.body.index(_assign)
    _target_table = _assign.targets[_table_value_idx]
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
        if isinstance(key, Name) and not f.between_brackets:
            # a.x  -> SetTabValue(a, "x", val)
            # a[x] -> SetTabValue(a,   x, val)
            key = String(key.id)
        settabvalue = SetTabValue(_target_table, key, f.value, parent=_assign.parent,
                                  first_token=n.first_token, last_token=n.last_token)
        _assign.parent.body.insert(_assign_idx+1, settabvalue)

def transform_literal_tables_to_assignments(tree):
    """
    Rewrite literal tables `a = {x=5, y=6}` or `return {x=5, y=6}` into
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
        if isinstance(n.parent, Assign):
            _literal_table_assign(n)
            continue
        elif isinstance(n.parent, Return):
            _literal_table_return(n)
        else:
            continue


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
            # given
            # b.key = value
            # -> find when `b` was created via assign, like
            # a, b, c = ?
            # TODO: Global/out-of-scope `b`/reassigned
            _tnames = [_t.id for _t in s.targets]
            if n.table.id not in _tnames:
                continue
            assert len(s.targets) == len(s.values)
            _tab_idx = _tnames.index(n.table.id)
            if not isinstance(s.values[_tab_idx], Table):
                # TODO ?? what's ArrayIndex doing
                continue
            d = n.key.dump()
            if d not in s.values[_tab_idx].field_names:
                s.values[_tab_idx].field_names.append(d)

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

        if isinstance(_assign.parent.body, Block):
            _body = _assign.parent.body.body
        else:
            _body = _assign.parent.body

        if not _assign in _body:
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
        assert n.parent
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
        assert n.parent
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
            n.parent.replace_child(n, FunctionReference(Name(_callable_name), n.environment))
        else:
            _callable_name = f"__nested_func_{n.name.id}" # FIXME: should include parent's name
            n.parent.replace_child(n, Assign([n.name], [FunctionReference(Name(_callable_name), n.environment)], parent=n.parent))

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

def _first_parent_containing_assign_of_or_argument(t: Node, n: Name, acceptable_scope=Scope) -> tuple[Optional[_Scope], bool]:
    """
    Returns (Node?, was-argument)
    """
    if t.parent is None:
        return None, False
    if isinstance(t.parent, acceptable_scope):
        _body = t.parent.body.body
    else:
        return _first_parent_containing_assign_of_or_argument(t.parent, n)

    # UpValues are "external local variables", we only look for LocalAssign
    for stmt in _body:
        if not isinstance(stmt, LocalAssign):
            continue
        if n.id not in [_t.id for _t in stmt.targets]:
            continue
        if not isinstance(t.parent, Fornum) and n.id in [_a.id for _a in t.parent.args]:
            continue
        return t.parent, False

    if isinstance(t.parent, Fornum):
        if t.parent.target.id == n.id:
            return t.parent, True

    # Function?
    if not isinstance(t.parent, Fornum):
        for arg in t.parent.args:
            if arg.id == n.id:
                return t.parent, True

    # didn't find a LocalAssign in the function, keep going up
    return _first_parent_containing_assign_of_or_argument(t.parent, n)

def _convert_local_name_to_lambda_env(block, name):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(block)
    for n in tree_visitor.nodes:
        if not isinstance(n, Name):
            continue
        # we are walking recursively down the tree, but only care for anything
        # directly within the scope of the original block; not for nested functions
        if _first_parent_of_type(n, FunctionLike) != block:
            continue
        assert n.parent, n.dump()
        if n.id != name.id:
            continue
        if isinstance(n.parent, Index) and n.parent.notation is IndexNotation.DOT:
            # when getting to `value.index`, if `index` matches, do not replace it
            if n == n.parent.idx:
                continue
        new_child = Index(Name(n.id), Name("lambda_args"), IndexNotation.DOT)
        n.parent.replace_child(n, new_child)

def _localassign_of_lambda_args(f: _Scope) -> Optional[Node]:
    _scopes = [f] + all_parent_scopes(f)

    for scope in _scopes:
        if isinstance(scope, Block):
            _body = scope.body
        else:
            _body = scope.body.body

        for n in _body:
            if not isinstance(n, LocalAssign):
                continue
            if len(n.targets) != 1:  # specifically looking for `local lambda_args = {}`
                continue
            t = n.targets[0]
            if not isinstance(t, Name) or t.id != "lambda_args":
                continue
            v = n.values[0]
            if not isinstance(v, Table):
                continue
            return n
    return None

def transform_captured_variables(tree):
    """
    Given a local variable that is used in a nested function ("UpValue"), replace it
    with a table-based access

    function a()                | function a()
        local var = 5           |   local lambda_args = {var=5}
        function b()            |   function b()
                                |       var = _lambda_environment.var
            return var          |       return var
        end                     |   end
        return b                |   return TCLOSURE(b, lambda_args)
    end                         | end

    Arguments to functions are also local variables
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if not isinstance(n, Name):
            continue
        # a LocalAssign can never be an UpValue
        if isinstance(n.parent, LocalAssign):
            continue
        if isinstance(n.parent, Fornum):
            continue
        if isinstance(n.parent, Field) and not n.parent.between_brackets and isinstance(n.parent.key, Name) and n.parent.key.id == n.id:
            # Do not consider literal table values with "string" keys variables that can be captured
            # a = {x=5}
            # x is not a variable
            continue
        if isinstance(n.parent, Index) and isinstance(n.parent.value, Index) and n.parent.value.value.id == "lambda_args" and n.parent.notation is IndexNotation.DOT:
            # When multiple variables in the same scope are captured, the scope
            # will be visited multiple times. After the first iteration, `lambda_args.captured`
            # itself looks like a captured argument, and would otherwise get captured again
            continue
        if isinstance(n.parent, Index) and n == n.parent.idx and n.parent.notation is IndexNotation.DOT:
            # When doing index accesses, such as `table.idx`, do not consider `idx` as usage of the name `idx`
            # in general, they may overlap with External-Local-ly variables
            continue
        defined_in, was_argument = _first_parent_containing_assign_of_or_argument(n, n)
        if not defined_in:
            continue

        _function_needing_upvalues = _first_parent_of_type(n, FunctionLike)
        if _function_needing_upvalues == defined_in:
            continue

        if isinstance(defined_in, Fornum) and _first_parent_of_type(defined_in, FunctionLike) == _function_needing_upvalues:
            """
            Fornum defines its iteration variable as a LocalAssign in its own body (Block).
            If the parent-function that needs the value is the same as the one that defines the ForNum,
            it's not really being used
            TODO: Fornum should only define its own variable as a LocalAssign when it's clear
            that it is captured
            """
            continue

        # 1. Replace all references to the original var with lambda_args.var
        _convert_local_name_to_lambda_env(defined_in, n)
        _convert_local_name_to_lambda_env(_function_needing_upvalues, n)

        # 2. Define a `lambda_args` table in the original environment
        # and a SetTabValue in the correct body
        _la = _localassign_of_lambda_args(defined_in)
        if _la is None:
            # lambda_args has not been locally defined in recursive parent scopes
            # we walk the scopes downwards, so if it was necessary it'd already be defined
            # => it should be defined as LocalAssign in the function
            _la = LocalAssign([Name("lambda_args")], [Table([])], parent=defined_in.body)
            # This should not get extra MakeTables
            defined_in.body.body.insert(0, _la)

        if was_argument:
            # copy the argument to the lambda-args
            if _la.parent == defined_in.body:
                f = Field(n.copy(), Name(n.id), between_brackets=False, parent=_la)
                _la.values[0].fields.append(f)
            else:
                # lambda_args _has_ been defined in a parent scope, we only need 
                # to assign a field value to it
                stv = SetTabValue(Name("lambda_args"), String(n.id), n.copy(), parent=defined_in.body)
                defined_in.body.body.insert(0, stv)

        # 3. Inject `lambda_args` as argument in the closure
        if not any(_a.id == "lambda_args" for _a in _function_needing_upvalues.args):
            _function_needing_upvalues.args.append(Name("lambda_args"))

        # 4. Mark creation of closure with `environment=lambda_args`
        _function_needing_upvalues.environment = Name("lambda_args")

def transform(src, pretty=True, dump_ast=False, testing_params=None):
    tree = ast.parse(src)
    set_parent_on_children(tree)

    rename_stdlib_calls(tree)
    transform_captured_variables(tree) # before transform_functions_to_vec_args, as it has to ignore args
                                       # before transform_anonymous_functions, as it has to walk upwards looking for UpValues
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
#include "stdlib.h"
#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
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
    p = subprocess.Popen(['clang-format', '--style={ColumnLimit: 150}'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    out, err = p.communicate(input=src.encode(), timeout=3)
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

    t = transform(src, pretty=True, dump_ast=False)
    print(t)

if __name__ == '__main__':
    main()
