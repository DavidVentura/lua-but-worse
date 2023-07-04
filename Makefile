.PHONY: test test_compiler

test: test_fix32 test_compiler

test_fix32: test_fix32.c lua.c fix32.h
	gcc -g -O0 -Wall -fsanitize=address test_fix32.c fix32.c -lm &&  ./a.out

test_compiler:
	./venv/bin/pytest tests.py
