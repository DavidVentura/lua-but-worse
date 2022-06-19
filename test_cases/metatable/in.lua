function main()
  a = {x=5}
  a.__index = a
  b = {}
  setmetatable(b, a)
  print(b.x)
  b.x += 5
  print(b.x)

  print(b.y)
  a.y = 7
  print(b.y)
  return 0
end
