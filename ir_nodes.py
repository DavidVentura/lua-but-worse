from dataclasses import dataclass, field
from typing import Optional, Union


@dataclass(frozen=True)
class CType:
    """C type representation"""
    name: str
    is_pointer: bool = False

    def __str__(self):
        return f"{self.name}*" if self.is_pointer else self.name


TVALUE = CType("TValue_t")  # TValue_t passed by value (6 bytes)
INT = CType("int")
DOUBLE = CType("double")
BOOL = CType("bool")
STRING = CType("const char", is_pointer=True)
VOID = CType("void")
TVALUE_ARRAY = CType("TValue_t", is_pointer=True)


@dataclass(frozen=True)
class CVar:
    """C variable"""
    name: str
    type: CType


@dataclass(frozen=True)
class CExpr:
    """Base for C expressions"""
    pass


@dataclass(frozen=True)
class CVarRef(CExpr):
    """C variable reference"""
    var: CVar


@dataclass(frozen=True)
class CLiteral(CExpr):
    """C literal value"""
    value: str
    type: CType


@dataclass(frozen=True)
class CFunctionCall(CExpr):
    """C function call (runtime helper or static function)"""
    func_name: str
    args: list[CExpr]


@dataclass(frozen=True)
class CBinOp(CExpr):
    """C binary operation"""
    op: str
    left: CExpr
    right: CExpr


@dataclass(frozen=True)
class CUnOp(CExpr):
    """C unary operation"""
    op: str
    operand: CExpr


@dataclass(frozen=True)
class CStmt:
    """Base for C statements"""
    pass


@dataclass(frozen=True)
class CDeclare(CStmt):
    """C variable declaration: TValue* x;"""
    var: CVar
    init: Optional[CExpr] = None


@dataclass(frozen=True)
class CAssign(CStmt):
    """C assignment: x = value;"""
    target: CVar
    value: CExpr


@dataclass(frozen=True)
class CIf(CStmt):
    """C if statement"""
    condition: CExpr
    then_stmts: list[CStmt]
    else_stmts: list[CStmt] = field(default_factory=list)


@dataclass(frozen=True)
class CFor(CStmt):
    """C for loop"""
    init: Optional[CStmt]
    condition: Optional[CExpr]
    increment: Optional[CStmt]
    body: list[CStmt]


@dataclass(frozen=True)
class CWhile(CStmt):
    """C while loop"""
    condition: CExpr
    body: list[CStmt]


@dataclass(frozen=True)
class CReturn(CStmt):
    """C return"""
    value: Optional[CExpr] = None


@dataclass(frozen=True)
class CExprStmt(CStmt):
    """Expression as statement"""
    expr: CExpr
    needs_cleanup: bool = True


@dataclass(frozen=True)
class CFunctionDef:
    """C function definition

    All Lua functions have signature: TValue* name(TValue** args, int argc)
    The 'params' field lists logical parameter names for extraction in the function body.
    The 'captures' field lists variables captured from outer scopes (for closures).
    """
    name: str
    params: list[str]  # Logical param names (extracted from args array)
    body: list[CStmt]
    captures: list[str] = field(default_factory=list)  # Captured variable names
    return_type: CType = TVALUE

    def is_closure(self) -> bool:
        """Returns True if this function has captures (is a closure)"""
        return len(self.captures) > 0


CExpr = Union[CVarRef, CLiteral, CFunctionCall, CBinOp, CUnOp]
CStmt = Union[CDeclare, CAssign, CIf, CFor, CWhile, CReturn, CExprStmt]
