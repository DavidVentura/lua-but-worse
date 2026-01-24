-- Test case 1: Simple redeclaration
function test_simple_redecl()
    local x = 1
    printh(x)  -- 1
    local x = 2
    printh(x)  -- 2
    local x = 3
    printh(x)  -- 3
end

-- Test case 2: Redeclaration with closure capture
function test_closure_capture()
    local c = 5
    local f = function() printh(c) end  -- captures first c
    local c = 6                         -- new c
    f()                                 -- should print 5
    printh(c)                           -- should print 6
end

-- Test case 3: Multiple closures capturing different variables
function test_multiple_closures()
    local x = 10
    local f1 = function() printh(x) end  -- captures first x (10)
    local x = 20
    local f2 = function() printh(x) end  -- captures second x (20)
    local x = 30
    f1()  -- 10
    f2()  -- 20
    printh(x)  -- 30
end

-- Test case 4: Redeclaration in same statement list
function test_sequential_redecl()
    local a = 100
    printh(a)
    local a = 200
    printh(a)
    local a = 300
    printh(a)
    local a = 400
    printh(a)
end

function main()
    test_simple_redecl()
    printh("---")
    test_closure_capture()
    printh("---")
    test_multiple_closures()
    printh("---")
    test_sequential_redecl()
    return 0
end
