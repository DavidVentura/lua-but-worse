function test_add()
	local t = {}
	add(t, "a value")
	printh(t[1])
	printh(t[2])
	add(t, "another value")
	printh(t[1])
	printh(t[2])
	printh(t[3])
end
function test_del2()
	local t = {1,2,3,4,5,6}
	printh(t[4]) -- 4
	del(t, 4)
	printh(t[4]) -- 5
	printh(t[5]) -- 6
	del(t, 4)
	printh(t[4]) -- 6
	printh(t[5]) -- nil
end
function test_del()
	local t = {}
	add(t, "a value")
	add(t, "another value")
	printh(t[1])
	del(t, "a value")
	printh(t[1])
end

function main()
	test_add()
	printh("---")
	test_del()
	printh("---")
	test_del2()
end
