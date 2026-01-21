#!/usr/bin/env python3

from lark import Lark
from pathlib import Path
from functools import lru_cache

from ast_builder import ASTBuilder
from code_generator import CCodeGenerator
from escape_analyzer import EscapeAnalyzer
from ir_lowering import IRLowering
from scope_resolver import ScopeResolver
from capture_detector import CaptureDetector


def load_grammar():
    grammar_path = Path(__file__).parent / "grammar.lark"
    return grammar_path.read_text()


@lru_cache
def create_parser():
    grammar = load_grammar()
    return Lark(grammar, parser='earley', start='start')


def parse(code):
    parser = create_parser()
    return parser.parse(code)


if __name__ == "__main__":
    import sys
    test_code = """
a = 5
function b()
    local x = 5
    c(x)
    -- return x
end
    """
    if len(sys.argv) > 1:
        test_code = open(sys.argv[1]).read()

    tree = parse(test_code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    # print(ast)
    s = ScopeResolver(ast)
    scopes, global_scope = s.analyze()
    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    e = EscapeAnalyzer(scopes, global_scope)
    e.analyze(ast)

    lowering = IRLowering(scopes, global_scope, e.escaping_vars)
    globals, functions = lowering.lower(ast)

    c = CCodeGenerator()
    code = c.generate(globals, functions)
    print(code)
