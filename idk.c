#include "lua.c"
void potato() {
	// as this function is a closure inside `global` scope,
	// it should read/write directly to global__UpValues
	set_tabvalue(ENV, &TSTR("c"), TNUM8(11));
	set_tabvalue(ENV, &TSTR("b"), TNUM8(7));
}
void thing2() {
	print(_mult(get_tabvalue(ENV, &TSTR("b")), TNUM8(3)));
	print(_add(get_tabvalue(ENV, &TSTR("b")),
			   get_tabvalue(ENV, &TSTR("c"))));
}

void global() {
	uint32_t a = 5;
	a = 6;
	a = 7;
}


int main() {
	ENV = make_table(0);
	global();
	potato();
	thing2();

	return 0;
}
