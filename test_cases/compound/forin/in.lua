function main()
	local tab = {a=1, b=1, c=2, d=3, e=5, f=8}
	for k, v in pairs(tab) do
		printh(k)
		printh(v)
	end
	printh("ipairs over fib table")
	-- this should be empty, as ipairs iterates over integer, contiguous indexes
	for k, v in ipairs(tab) do
		printh(k)
		printh(v)
	end

	printh("ipairs over num table")
	local numtab = {123, 456, n=42, x=100, y=200, 789}
	for k, v in ipairs(numtab) do
		printh(v)
	end
end
