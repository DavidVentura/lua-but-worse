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
end
