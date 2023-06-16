function main()
	t = {}
	print(getmetatable(t))
	t1 = {}
	setmetatable(t, t1)
	print(getmetatable(t) == t1)
end
