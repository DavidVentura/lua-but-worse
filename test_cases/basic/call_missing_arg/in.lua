function f(a, b, c)
	print(a)
	print(b)
	print(c)
end
function main()
	print("all")
	f(1,2,3)
	print("2 missing")
	f(1)
	print("all missing")
	f()
end
