import sys
import textwrap
import subprocess

from luaparser import ast
from luaparser.astnodes import Assign, LocalAssign, Index, Function, Type, Call

# TODO: broken parsing when declaring local variables with no value:
# ```
# local j
# ```

def add_decls(tree):
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
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = []
    for n in tree_visitor.nodes:
        if not isinstance(n, Call):
            continue
        if n.func.id in ['sin', 'cos']:
            n.func.id = f'fix32::{n.func.id}'

def add_signatures(tree):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = []
    for n in tree_visitor.nodes:
        if not isinstance(n, Function):
            continue
        # magical 3 indents = root; need a better way to figure out if a function
        # lives inside another function
        if n.parent.parent.parent is not None:
            continue
        tree.body.add_signatures(n)

def transform(src, pretty=True, dump_ast=False, testing=False):
    tree = ast.parse(src)
    ret = '#include "header.h"\n'
    ret += 'namespace Game {\n'
    rename_stdlib_calls(tree)
    add_signatures(tree)
    add_decls(tree)

    if dump_ast:
        print(ast.to_pretty_str(tree))

    ret += tree.body.dump()
    ret += '}\n'
    if testing:
        ret += textwrap.dedent('''
        int main() {
            Game::main();
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
