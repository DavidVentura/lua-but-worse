function test_double_enclosing_fornum_iterator()
	printh("double_enclosing fornum iterator")
	for x=1,2 do
		for y=1,2 do
			function whatever_func()
				printh(x*y)
			end
			whatever_func()
		end
	end
end
function test_enclosing_fornum_iterator()
	printh("enclosing fornum iterator")
	for i=1,2 do
		function func()
			printh(i)
		end
		func()
	end
end

function test_enclosing_table_index()
	printh("enclosing arg overlap")
	function wrapper(x,y)
		local obj = {}
		obj.x = "not wrapped"
		obj.a = "wrapped"
		function wrapped()
			printh(obj.x)  -- this 'x' should NOT be wrapped
			printh(obj[y]) -- this 'y' should be wrapped
		end
		wrapped()
	end
	wrapper(5, "a")
end
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
  test_enclosing_table_index()
  test_enclosing_fornum_iterator()
  test_double_enclosing_fornum_iterator()
  return 0
end
