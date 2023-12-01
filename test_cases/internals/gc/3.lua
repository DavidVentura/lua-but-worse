function _references_in_table_destroyed()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	local tab = {}
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
	tab[0] = {}
	__internal_debug_assert_eq(__internal_debug_tables_used(), 2)
	run_gc() -- this only decref's
	__internal_debug_assert_eq(__internal_debug_tables_used(), 2)
end

function test_references_in_table_destroyed()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	_references_in_table_destroyed()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
end

function main()
	test_references_in_table_destroyed()
end
