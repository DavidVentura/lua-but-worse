function make_obj()
    return {x=10, y=20}
end

function main()
    local obj1 = make_obj()
    obj1.x = -1
    printh(obj1.x)

    local obj2 = make_obj()
    printh(obj2.x)

    local obj3 = make_obj()
    obj3.y = 99
    printh(obj3.y)

    return 0
end
