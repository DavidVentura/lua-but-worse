vector = {}
vector.__index = vector

function vector.new(x, y)
	local o = {x=x, y=y}
	setmetatable(o, vector)
	return o
end

function vector.__add(v0, v1)
	local v = vector.new(0, 0)
	v.x = v0.x + v1.x
	v.y = v0.y + v1.y
	return v
end

function vector.__sub(v0, v1)
	local v = vector.new(0, 0)
	v.x = v0.x - v1.x
	v.y = v0.y - v1.y
	return v
end

function main()
	local v1 = vector.new(5, 3)
	local v2 = vector.new(1, 2)
	local delta = v1 - v2
	printh(delta.x)
	printh(delta.y)
	local added = v1 + v2
	printh(added.x)
	printh(added.y)
end
