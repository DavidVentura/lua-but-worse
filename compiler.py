#!/usr/bin/env python3

from lark import Lark, Tree
from pathlib import Path


def load_grammar():
    grammar_path = Path(__file__).parent / "grammar.lark"
    return grammar_path.read_text()


def create_parser():
    grammar = load_grammar()
    return Lark(grammar, parser='lalr', start='start')


def parse(code):
    parser = create_parser()
    return parser.parse(code)


if __name__ == "__main__":
    test_code = "a = 5"

    tree = parse(test_code)
    print(tree.pretty())
