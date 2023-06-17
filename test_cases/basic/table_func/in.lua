vector = {}

function vector.new(x, y)
	local o = {x=x, y=y}
	setmetatable(o, vector)
	return o
end

function main()
	local zero = vector.new(0, 0)
	local one = vector.new(1, 1)
	print(zero.x)
	print(one.x)
end
