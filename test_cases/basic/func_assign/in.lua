g_obj = nil

function make_obj()
    g_obj = {x=10, y=20}
    return g_obj
end

function main()
    make_obj().x = -1
    printh(g_obj.x)

    make_obj().y = 99
    printh(g_obj.y)

    return 0
end
