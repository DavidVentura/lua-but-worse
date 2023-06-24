function main()
	local intermediate = nil
	for i=1,10 do
		intermediate = tostring(i)
		s = "#"..intermediate
	end
	-- at the end of this loop, only 1 string should exist, "100"
	-- but 3 strings exist: "#100" "#" "100"
	-- with an auto-free on scope-end, there should be 0 values left
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
end
