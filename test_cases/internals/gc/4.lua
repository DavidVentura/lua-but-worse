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
	deleted_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
end
