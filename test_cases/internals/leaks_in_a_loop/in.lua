function _inner()
	local s = "$"
	local intermediate
	for i=1,10 do
		-- intermediate = tostring(i)
		s = "#"..i -- tostring(i)
	end
	-- 3 strings exist: "#10" "#" "10" and "$"
	-- i would expect "10" to not exist
	__internal_debug_assert_eq(__internal_debug_str_used(), 4)
end
function main()
	-- 2 const strings exist -- "#" and "$"
	__internal_debug_assert_eq(__internal_debug_str_used(), 2)
	_inner()
	-- locals were GC'd
	__internal_debug_assert_eq(__internal_debug_str_used(), 2)
end
