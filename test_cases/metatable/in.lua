function main()
  a = {x=5}
  a.__index = a
  b = {}
  setmetatable(b, a)
  print(b.x)
  b.x += 5
  print(b.x)
  return 0
end
