import textwrap

from luaparser import ast
from luaparser.astnodes import Assign, LocalAssign, Index, Function
#from luaparser.astnodes import LocalFunction, Function, Statement, Call, Expression, String, Assign, If, Number, Block, SubOp, AddOp, Name, ElseIf

# TODO: broken parsing when declaring local variables with no value:
# ```
# local j
# ```
# broken parsing when declaring string-based bracket access to a table
#  player["sbr"]=118

src = """
x = 64
y = 64
a = "some text"
b = "other value"
local j = 5
function _update()
  q = 5
  if (btn(0)) then x=x-1 end
  if (btn(1)) then x=x+1 end
  if (btn(2)) then y=y-1 end
  if (btn(3)) then y=y+1 end
end  

function _draw()
  print(q)
--  print(a+b)
  cls()
  map(0,0,0,0,16,8)
  map(0,0,0,64,16,9)
  spr(2,x,y)
end
"""

with open('tennis.lua') as fd:
    src = fd.read()
tree = ast.parse(src)
print('#include "header.h"')

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

add_signatures(tree)
add_decls(tree)
print(tree.body.dump())
