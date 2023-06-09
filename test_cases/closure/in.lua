function main()
	captured = 7
	a = function(x)
		return x*captured
	end
	print(a(5))

	function b()
		captured = 1
	end
	b()
	print(a(5))

	return 0
end

