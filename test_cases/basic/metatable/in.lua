function main()
	t = {}
	printh(getmetatable(t))
	t1 = {}
	setmetatable(t, t1)
	printh(getmetatable(t) == t1)
end
