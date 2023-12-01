function f(a, b, c)
	printh(a)
	printh(b)
	printh(c)
end
function main()
	printh("all")
	f(1,2,3)
	printh("2 missing")
	f(1)
	printh("all missing")
	f()
end
