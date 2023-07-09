function newCounter ()
  local i = 0
  return function ()   -- anonymous function
    i = i + 1
    return i
  end
end

function main()
  local captured = 7
  a = function(x)
    return x*captured
  end
  printh(a(5))

  captured = 1
  printh(a(5))
  function b()
    captured = 2
  end
  b()
  printh(a(5))

  printh("newCounter")
  c1 = newCounter()
  printh(c1())  --> 1
  printh(c1())  --> 2
  c2 = newCounter()
  printh(c2())  --> 1
  printh(c1())  --> 3
  printh(c2())  --> 2
  return 0
end

