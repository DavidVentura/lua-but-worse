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
function main()
  test_double_enclosing_fornum_iterator()
end
