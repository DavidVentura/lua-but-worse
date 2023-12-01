function elseif_else_arm()
	local a
	if false then
		-- making sure A is part of the else arm and not if
		printh("nope")
	elseif false then
		printh("nope2")
	else
		local a = 200
		printh(a)
	end
	printh(a)
end
function else_arm()
	local a
	if false then
		-- making sure A is part of the else arm and not if
		printh("nope")
	else
		local a = 100
		printh(a)
	end
	printh(a)
end
function local_var()
	local a
	if 5 then
		local a = 100
	end
	printh(a) -- a 
end
function main()
	local_var()
	printh("---")
	else_arm()
	printh("---")
	elseif_else_arm()
end
