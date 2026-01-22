#!/usr/bin/env python3

import pytest
from pathlib import Path
from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from capture_detector import CaptureDetector
from escape_analyzer import EscapeAnalyzer
from ast_normalizer import ASTNormalizer
from ir_lowering import IRLowering
from code_generator import CCodeGenerator


TEST_CASES_DIR = Path(__file__).parent / "test_cases"


def get_test_files():
    if not TEST_CASES_DIR.exists():
        return []

    test_files = []
    for lua_file in TEST_CASES_DIR.rglob("in.lua"):
        test_files.append((lua_file.parent.name, lua_file))
    return sorted(test_files)


@pytest.mark.parametrize("name,lua_file", get_test_files())
def test_parse_real_case(name, lua_file):
    code = lua_file.read_text()

    # Parse
    parser = create_parser()
    tree = parser.parse(code)
    assert tree.data == "start"
    assert tree is not None

    # Build AST
    builder = ASTBuilder()
    ast = builder.transform(tree)

    # Scope resolution
    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    # Capture detection
    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    # Escape analysis
    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    # Normalize AST
    normalizer = ASTNormalizer(scopes, global_scope)
    ast = normalizer.normalize(ast)

    # Lower to IR
    lowering = IRLowering(scopes, global_scope, analyzer.escaping_vars)
    globals, functions, escaping_names = lowering.lower(ast)

    # Generate C code
    codegen = CCodeGenerator()
    codegen.generate(globals, functions, escaping_names)
