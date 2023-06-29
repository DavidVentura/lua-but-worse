function main()
	a = 4 and 5 and 6
	printh(a)         --> 5
	a = nil and 13
    printh(a)      --> nil
	a = false and 13
    printh(a)    --> false
	a = 4 or 5
    printh(a)          --> 4
	a = false or 5
    printh(a)      --> 5
	printh("----")
	-- shortcut logic, it is important to have function calls
	-- on the right of the and/or operator to prove that
	-- they are not evaluated
	a = 5 and printh("visible")
	a = 5 and printh("visible2") and printh("also visible")
	a = false and printh("NOT visible") and printh("also NOT visible")
    a = false and printh("AND-NOT VISIBLE")
    a = true or printh("OR-NOT VISIBLE")

	if 4 and 5 and 6 then
		printh("yep")
	end
	-- `and` becomes
	-- let and_result = first
	-- if first then
	-- 	  and_result = second
	-- end
	-- keeping in mind that `first` and `second` may be expressions that only have
	-- to be evaluated once and stored in a temp var
	--
	--
	-- `or` becomes
	-- let or_result = first
	-- if !first then
	-- 	  and_result = second
	-- end
	-- keeping in mind that `first` and `second` may be expressions that only have
	-- to be evaluated once and stored in a temp var
end
