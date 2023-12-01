function main()
    local x = 1
    x += 2
    a, b = 1, 2
    c, d = flr(1.5), flr(2.5)
    e = 1
    e = 2
    o = {}
    u = {a=1}
    o.a = u.a

    t = 7
    t = ({[7]=8,[8]=7})[t]
    printh(a*c)
    printh(b*d)
    printh(e)
    printh(o.a)
    printh(t)
    return 0
end
