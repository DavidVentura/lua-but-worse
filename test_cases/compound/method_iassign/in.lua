vector = {}
vector.__index = vector

function vector:len()
	return sqrt(sqr(self.x) + sqr(self.y))
end

function vector:normal()
	self.x /= self:len()
	self.y /= self:len()
end

function vector.new(x, y)
	local o = {x=x, y=y}
	setmetatable(o, vector)
	return o
end

function main()
	local zero = vector.new(0, 0)
	print(zero:len())
	local one = vector.new(1, 1)
	print(one:len())
end
