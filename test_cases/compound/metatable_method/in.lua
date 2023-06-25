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
  printh(a.x)
  printh(b.x)
  a:method()
  printh(a.x)
  printh(b.x)
end
