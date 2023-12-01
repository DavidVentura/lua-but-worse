function _inner_dyn()
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
	local a = "hi"
	local b = tostring(3)
	__internal_debug_assert_eq(__internal_debug_str_used(), 4)
	local c = a..b
	__internal_debug_assert_eq(__internal_debug_str_used(), 5)
end
function _inner()
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
	local a = "hi"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
	local b = "2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
	local c = "hi".."2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3) -- "hi2" is a KNOWN str (next)
	local c = "hi2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
end

function main()
	_inner()
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
	-- this is testing that local variables get collected once they fall out of scope
	_inner_dyn()
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
end
