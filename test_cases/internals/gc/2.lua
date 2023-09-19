function returned_tables_1()
	local var = {field=1}
	return var
end

function _nested_returned_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	local t = returned_tables_1()
	run_gc() -- this GC should drop the tmep `var`, bringing refcount from 2 to 1
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
end
function nested_returned_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	_nested_returned_tables() -- as the last refcount for `var` is `t`, it should die when it goes out of scope
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
end

function main()
	nested_returned_tables()
end
