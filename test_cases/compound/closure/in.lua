function celeste()
	local var = 7
	local tab = {1, 2, 3}
	foreach(tab,
		function(item)
			printh(item*var)
		end
	)
end
function main()
	celeste()
	captured = 7
	a = function(x)
		return x*captured
	end
	printh(a(5))

	function b()
		captured = 1
	end
	b()
	printh(a(5))

	return 0
end

