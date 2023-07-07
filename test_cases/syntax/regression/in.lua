function bool_weird()
 if printh(0) and mult_or(1, 1) then
	a = 1
 elseif printh(1111) and mult_or(4444, 5555) then
	a = 1
 end
end
function mult_or()
	local w = 2
	local sw = (w or 1)*8
end
function or_call()
  local obj = false
  (obj or nil)(obj)
end
function fractional_binary_literal()
	printh("fractional_binary_literal")
	printh(0b0111110101111101.1) -- 32125.5
end
function mod_equal()
	a %= 5
end
function inplace_arith_for_bracket_table_assign()
	obj = {}
	axis = 5
	obj.rem[axis] += axis
end
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
	fractional_binary_literal()
end
