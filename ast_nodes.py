from dataclasses import dataclass, field
from typing import Optional, Union
from enum import Enum


@dataclass(frozen=True)
class NameRef:
    """Variable reference: a"""
    name: str
    resolved: Optional['VarInfo'] = None


@dataclass(frozen=True)
class Number:
    """Number literal: 5, 3.14, 0x10"""
    value: str


@dataclass(frozen=True)
class String:
    """String literal: "hello" """
    value: str


@dataclass(frozen=True)
class Bool:
    """Boolean literal: true, false"""
    value: bool


@dataclass(frozen=True)
class Nil:
    """Nil literal"""
    pass


@dataclass(frozen=True)
class BinOp:
    """Binary operation: a + b"""
    op: str
    left: 'Expr'
    right: 'Expr'


@dataclass(frozen=True)
class UnOp:
    """Unary operation: -a, not b, #t"""
    op: str
    operand: 'Expr'


@dataclass(frozen=True)
class TableConstructor:
    """Table literal: {1, 2, x=3}"""
    fields: list['TableField']


@dataclass(frozen=True)
class TableField:
    """Single table field"""
    key: Optional['Expr']
    value: 'Expr'


@dataclass(frozen=True)
class TableAccess:
    """Table access: t.x or t[expr]"""
    table: 'Expr'
    key: 'Expr'
    is_dot: bool


@dataclass(frozen=True)
class FunctionCall:
    """Function call: f(a, b)"""
    func: 'Expr'
    args: list['Expr']


@dataclass(frozen=True)
class MethodCall:
    """Method call: obj:method(a, b)"""
    obj: 'Expr'
    method: str
    args: list['Expr']


@dataclass(frozen=True)
class AnonymousFunction:
    """Anonymous function: function(x) ... end"""
    params: list[str]
    body: 'Block'
    scope_id: Optional[int] = None


@dataclass(frozen=True)
class Block:
    """Block of statements"""
    stmts: list['Stmt']


@dataclass(frozen=True)
class Assign:
    """Assignment: a, b = 1, 2"""
    targets: list['Expr']
    values: list['Expr']


@dataclass(frozen=True)
class CompoundAssign:
    """Compound assignment: a += 5"""
    target: 'Expr'
    op: str
    value: 'Expr'


@dataclass(frozen=True)
class LocalDecl:
    """Local declaration: local a, b = 1, 2"""
    names: list[str]
    values: list['Expr']


@dataclass(frozen=True)
class FunctionDef:
    """Function definition: function f(x) ... end"""
    name: list[str]
    is_method: bool
    params: list[str]
    body: 'Block'
    scope_id: Optional[int] = None


@dataclass(frozen=True)
class If:
    """If statement"""
    condition: 'Expr'
    then_block: 'Block'
    elseif_parts: list[tuple['Expr', 'Block']]
    else_block: Optional['Block']


@dataclass(frozen=True)
class ForNum:
    """Numeric for: for i=1,10,2 do ... end"""
    var: str
    start: 'Expr'
    stop: 'Expr'
    step: Optional['Expr']
    body: 'Block'
    scope_id: Optional[int] = None


@dataclass(frozen=True)
class ForIn:
    """Iterator for: for k, v in pairs(t) do ... end"""
    vars: list[str]
    iterator: 'Expr'
    body: 'Block'
    scope_id: Optional[int] = None


@dataclass(frozen=True)
class While:
    """While loop: while cond do ... end"""
    condition: 'Expr'
    body: 'Block'


@dataclass(frozen=True)
class Return:
    """Return statement: return a, b"""
    values: list['Expr']


@dataclass(frozen=True)
class ExprStmt:
    """Expression as statement: f(x)"""
    expr: 'Expr'


Expr = Union[NameRef, Number, String, Bool, Nil,
              BinOp, UnOp, TableConstructor, FunctionCall,
              TableAccess, AnonymousFunction, MethodCall]

Stmt = Union[Assign, LocalDecl, FunctionDef, If,
              ForNum, ForIn, While, Return, ExprStmt, CompoundAssign]


class VarKind(Enum):
    LOCAL = "local"
    PARAM = "param"
    GLOBAL = "global"


@dataclass(frozen=True)
class VarInfo:
    """Information about a variable"""
    name: str
    scope_id: int
    kind: VarKind


@dataclass
class Scope:
    """A lexical scope (function, for loop, etc.)"""
    scope_id: int
    parent: Optional['Scope']
    is_function: bool = False
    is_loop: bool = False
    vars: dict[str, VarInfo] = field(default_factory=dict)
    captures: set[VarInfo] = field(default_factory=set)
