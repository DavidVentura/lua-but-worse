function returned_tables_1()
	local var = {field=1}
	return var
end

function returned_tables_2()
	return {field=1}
end

function returned_tables_3()
	return {}
end

function main()
	local t = returned_tables_1()
	printh(t.field)
	local t2 = returned_tables_2()
	printh(t2.field)
	local t3 = returned_tables_3()
	printh(#t3)
end
