function test_enclosing_fornum_iterator()
	printh("enclosing fornum iterator")
	for i=1,2 do
		function func()
			printh(i)
		end
		func()
	end
end

function main()
  test_enclosing_fornum_iterator()
end
