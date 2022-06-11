function main()
	squares = {1, 4, 9, 16, 25}
	member  = {x=1}
	empty   = {}

	-- using table as index
	member[empty] = 5
	print(member[empty])
end
