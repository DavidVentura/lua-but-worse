import sys
import textwrap
import subprocess

from luaparser import ast
from luaparser.astnodes import Assign, LocalAssign, Index, Function, Type
#from luaparser.astnodes import LocalFunction, Function, Statement, Call, Expression, String, Assign, If, Number, Block, SubOp, AddOp, Name, ElseIf

# TODO: broken parsing when declaring local variables with no value:
# ```
# local j
# ```

def add_decls(tree):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = []
    for n in tree_visitor.nodes:
        if isinstance(n, LocalAssign):
            for t in n.targets:
                if isinstance(t, Index):
                    continue
                pair = (t.scope(), t)
                if pair not in seen:
                    seen.append(pair)
        elif isinstance(n, Assign):
            for t in n.targets:
                if isinstance(t, Index):
                    continue
                pair = (tree.body, t)
                if pair not in seen:
                    seen.append(pair)

    for block, name in seen:
        block.add_declaration(name)

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

def patch_main_type(tree):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = []
    for n in tree_visitor.nodes:
        if not isinstance(n, Function):
            continue
        if not n.name.id == 'main':
            continue
        n.ret_type = Type.TESTING_INT

def transform(src, pretty=True, dump_ast=False, testing=False):
    tree = ast.parse(src)
    ret = '#include "header.h"\n'
    if testing:
        patch_main_type(tree)
    add_signatures(tree)
    add_decls(tree)

    if dump_ast:
        print(ast.to_pretty_str(tree))

    ret += tree.body.dump()
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
