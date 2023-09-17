function test_celeste_init_object_captures_obj()             
	local obj = {}
	obj.move_y=function(amount)
		obj.y = 0
		for i=0,2 do
			-- TODO: `obj` not captured in forloop
			obj.x = 0
		end
	end
end


function main()
	test_celeste_init_object_captures_obj()
end
