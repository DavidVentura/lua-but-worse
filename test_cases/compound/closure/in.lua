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

	return 0
end

