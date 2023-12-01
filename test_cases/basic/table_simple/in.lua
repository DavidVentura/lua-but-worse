function main()
	squares = {1, 4, 9, 16, 25}
	member  = {x=-1.5, y=66}
	empty   = {}

	printh(squares[2])
	printh(member.x)
	printh(member["y"])

	adder 	= {}
	adder.a = 1
	printh(adder.a)
	adder.b = 2
	printh(adder.a)
	printh(adder.b)
	adder.c = 3
	printh(adder.a)
	printh(adder.b)
	printh(adder.c)
	adder.d = 4
	printh(adder.a)
	printh(adder.b)
	printh(adder.c)
	printh(adder.d)

end
