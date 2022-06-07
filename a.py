import textwrap

from luaparser import ast
from luaparser.astnodes import Assign, LocalAssign, Index, Function
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

def transform(src):
    tree = ast.parse(src)
    ret = '#include "header.h"\n'
    add_signatures(tree)
    add_decls(tree)
    ret += tree.body.dump()
    return ret

#with open('tennis.lua') as fd:
with open('squares.lua') as fd:
    src = fd.read()

print(transform(src))
