function _inner()
	local s = "$"
	local intermediate
	for i=1,10 do
		-- intermediate = tostring(i)
		s = "#" .. tostring(i)
	end
	run_gc()
	-- 3 strings exist: "#10" "#" and "$"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
end
function main()
	-- 2 const strings exist -- "#" and "$"
	__internal_debug_assert_eq(__internal_debug_str_used(), 2)
	_inner()
	-- locals ("#10") were removed by refcount (scope)
	__internal_debug_assert_eq(__internal_debug_str_used(), 2)
end
