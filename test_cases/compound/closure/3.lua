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

function main()
  test_enclosing_table_index()
end
