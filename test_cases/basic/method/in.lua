function main()
  a = {}
  function a:method(value)
    self.x = value
    return nil
  end
  
  printh(a.x)
  a:method(5)
  printh(a.x)
end
