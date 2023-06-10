function main()
	squares = {1, 4, 9, 16, 25}
	member  = {x=1}
	empty   = {}
        literals = {
            ["top"] = {x=0, y=-68},
            ["bottom"] = {x=0, y=68},
            ["left"] = {x=-68, y=0},
            ["right"] = {x=68, y=0}
        }
	print(literals.top.y)

	-- using table as index
	member[empty] = 5
	print(member[empty])
end
