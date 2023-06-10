function main()
	literals = {
		["top"] = {x=0, y=-68},
		["bottom"] = {x=0, y=68},
		["left"] = {x=-68, y=0},
		["right"] = {x=68, y=0}
	}
	print(literals.top.y)
	print(literals.right.x)
end
