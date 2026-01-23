function deleted_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	local tab = {}
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
	for i = 1,5 do
		tab[i] = {}
	end
	__internal_debug_assert_eq(__internal_debug_tables_used(), 6)
	for i = 1,5 do
		del(tab, 1)
	end
	__internal_debug_assert_eq(__internal_debug_tables_used(), 1)
end

function main()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
	deleted_tables()
	__internal_debug_assert_eq(__internal_debug_tables_used(), 0)
end
