import string
import logging
import subprocess
import sys
import textwrap

from luaparser import ast
from luaparser.astnodes import Assign, LocalAssign, Index, Function, Type, Call, String, Name, IndexNotation, Number, Method, Invoke, Block

# TODO: broken parsing when declaring local variables with no value:
# ```
# local j
# ```

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger()

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

def transform_methods(tree):
    """
    Rewrite methods (`function tab:method() ... end`) into an assignment of lambda into a table
    ```
    tab["method"] = lambda(...)
    ```

    And method calls (`tab:method()`) into table calls with a self-argument (`tab['method'](tab)`)

    Further passes will optimize this access into a fast-field access.
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if isinstance(n, Method):
            replacement = n.replace_with_assign()
            idx = n.parent.body.index(n)
            n.parent.body[idx] = replacement
        if isinstance(n, Invoke):
            replacement = n.replace_with_idx_call()
            n.parent.replace_child(n, replacement)


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

    seen = []
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

def transform(src, pretty=True, dump_ast=False, testing=False):
    tree = ast.parse(src)
    rename_stdlib_calls(tree)
    transform_methods(tree)
    move_to_preinit(tree)
    add_signatures(tree)
    add_decls(tree)
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
