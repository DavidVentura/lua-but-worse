function _inner()
	local intermediate = nil
	local s = "$"
	intermediate = 5
	for i=1,10 do
		intermediate = tostring(i)
		s = "#"..intermediate
	end
	-- at the end of this loop, only 1 string should exist, "100"
	-- but 3 strings exist: "#100" "#" "100"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
end
function main()
	_inner()
	-- FIXME: this should be 0, but the anonymous "#" in concat
	-- is not cleaned up properly; TSTR() doesn't set refcount to 1
	-- _and_ it does not fall out of scope, as it's not defined
	-- as a local `gc` variable
	__internal_debug_assert_eq(__internal_debug_str_used(), 1)
end
