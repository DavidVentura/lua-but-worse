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
  test_function_args_captured()
end
