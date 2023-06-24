function _inner()
	__internal_debug_assert_eq(__internal_debug_str_used(), 0)
	local a = "hi"
	__internal_debug_assert_eq(__internal_debug_str_used(), 1)
	local b = "2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 2)
	local c = "hi".."2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3) -- "hi2" is a new str
	local c = "hi2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
end

function main()
	_inner()
	-- this is testing that local variables get collected once they fall out of scope
	__internal_debug_assert_eq(__internal_debug_str_used(), 0)
end
