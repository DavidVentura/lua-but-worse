-- pragma: disable_grow_table
function main()
	local a = {}  -- should make_table(1)
	a.a = 1
	local b = {x=1, y=2} -- should make_table(2)
	local c = {x=1, y=2} -- should make_table(3)
	c.a = 1
end
