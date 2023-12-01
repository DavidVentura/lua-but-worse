function test_returning_lambda()
  printh("newCounter")
  c1 = newCounter()
  printh(c1())  --> 1
  printh(c1())  --> 2
  c2 = newCounter()
  printh(c2())  --> 1
  printh(c1())  --> 3
  printh(c2())  --> 2
end

function newCounter ()
  local i = 0
  return function ()   -- anonymous function
    i = i + 1
    return i
  end
end

function main()
  test_returning_lambda()
end
