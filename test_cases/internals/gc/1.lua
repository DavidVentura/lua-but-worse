function returned_tables_1()
	local var = {field=1}
	return var
end

function returned_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	local t = returned_tables_1()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
end

function main()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	returned_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
	run_gc()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
end
