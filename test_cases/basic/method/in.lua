function main()
  a = {}
  function a:method(value)
    self.x = value
    return nil
  end
  
  print(a.x)
  a:method(5)
  print(a.x)
end
