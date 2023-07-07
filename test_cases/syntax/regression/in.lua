function short_while()
	while (false) a = 5
end
function peeks()
--	printh(@0x4300) -- 1
--	printh(%0b1111) -- 2
--	printh($0x4300) -- 4
end
function integer_div()
	printh(7\5)
	printh(8\5)
	printh(11\5)
end
--function bunny3(t, ...)
--	?"\^i"..t.."\^-i\^g\f0"..t,...
--end
function bunny2()
	-- something after return
	if (false) return
	a = 1
end

function bunny()
	-- short print
 	--?"arg1","arg2"
end
function celeste()
	-- bare return
	if (false) return
end

function main()
	celeste()
	integer_div()
	bunny()
	bunny()
end
