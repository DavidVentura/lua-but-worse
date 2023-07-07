function main()
	a = {}
	a.x, a.y = 5, 6
	printh(a.x)
	printh(a.y)
	b, c = 1, 2
	printh(b)
	printh(c)

	printh("tables")
	state, next_state, change_state = {a=1}, {b=2}, false
	printh(state.a)
	printh(next_state.b)
	printh(change_state)
end
