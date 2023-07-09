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

function test_function_args_captured()
	printh("captured args")
	function f(arg)
		function f2()
			printh(arg)
		end
		return f2
	end
	captures_5 = f(5)
	captures_6 = f(6)
	captures_5()
	captures_6()
	captures_5()
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

  test_returning_lambda()
  test_function_args_captured()
  return 0
end

