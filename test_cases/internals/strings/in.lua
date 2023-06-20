function main()
	__internal_debug_assert_eq(__internal_debug_str_used(), 0)
	a = "hi"
	__internal_debug_assert_eq(__internal_debug_str_used(), 1)
	b = "2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 2)
	c = "hi".."2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3) -- "hi2" is a new str
	c = "hi2"
	__internal_debug_assert_eq(__internal_debug_str_used(), 3)
end
