function _inner()
	local intermediate = nil
	local s = "$"
	intermediate = 5
	for i=1,10 do
		intermediate = tostring(i)
		s = "#"..intermediate
	end
	-- 4 strings exist: "#10" "#" "10" and "$"
	__internal_debug_assert_eq(__internal_debug_str_used(), 4)
end
function main()
	-- 2 const strings exist -- "#" and "$"
	__internal_debug_assert_eq(__internal_debug_str_used(), 2)
	_inner()
	-- locals were GC'd
	__internal_debug_assert_eq(__internal_debug_str_used(), 2)
end
