function main()
  a = {}
  a.__index = a
  function a:method()
    self.x = 5
    return nil
  end
  
  b = {}
  setmetatable(b, a)
  b:method()
  print(a.x)
  print(b.x)
  a:method()
  print(a.x)
  print(b.x)
end
