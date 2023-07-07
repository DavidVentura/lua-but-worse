function main()
    local y -- not assigned anything
    local x = 1
    x += 2
    a, b = 1, 2
    c, d = flr(1.5), flr(2.5)
    e = 1
    e = 2
    o = {}
    u = {a=123}
    o.a = 333
    local oof = .5

    printh(a*c)
    printh(b*d)
    printh(e)
    printh(u.a)
    printh(o.a)
    printh(y)
    return 0
end
