#!/usr/bin/env python3

from lark import Lark
from pathlib import Path
from functools import lru_cache


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
    test_code = "a = 5"

    tree = parse(test_code)
    print(tree.pretty())
