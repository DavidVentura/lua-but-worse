function main()
    local o = {} -- o is local, should be freed
	o.key = "value"
    u = {a=1} -- u is global, should not get freed
    t = ({[7]=8,[8]=7})[7] -- t uses an "anonymous table"
						   -- which should be created locally and freed
	printh(o.key)
	printh(u.a)
	printh(t)
    return 0
end
