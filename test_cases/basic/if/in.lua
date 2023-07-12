function cond(a,b,c)
	if a then
		printh("a")
	elseif b then
		printh("b")
	else
		printh("c")
	end
end
function main()
	cond(true,  false, false)
	cond(false, true,  false)
	cond(false, false, true)
end
