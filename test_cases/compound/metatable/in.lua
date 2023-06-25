function main()
  a = {x=5}
  a.__index = a
  b = {}
  setmetatable(b, a)
  printh(b.x)
  b.x += 5
  printh(b.x)

  printh(b.y)
  a.y = 7
  printh(b.y)
  return 0
end
