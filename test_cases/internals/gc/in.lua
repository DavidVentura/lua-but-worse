function returned_tables_1()
	local var = {field=1}
	return var
end

function returned_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	local t = returned_tables_1()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
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

function _references_in_table_destroyed()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	local tab = {}
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
	tab[0] = {}
	__internal_debug_assert_eq(__internal_debug_tables_used(), 2)
	run_gc() -- this only decref's
	__internal_debug_assert_eq(__internal_debug_tables_used(), 2)
end

function references_in_table_destroyed()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	deleted_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
end

function deleted_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	local tab = {}
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
	for i = 1,5 do
		tab[i] = {}
	end
	run_gc() -- runs decref on all `{}`
	__internal_debug_assert_eq(__internal_debug_tables_used(), 6)
	for i = 1,5 do
		del(tab, 1)
	end
	run_gc() -- executes the scheduled decref on every elem from tab
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1) -- `tab`
end

function main()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	returned_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
	run_gc()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)

	nested_returned_tables()

	---
	references_in_table_destroyed()

	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	deleted_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
end
