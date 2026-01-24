-- Test that if/else branches create proper scopes
-- Variables declared in different branches should be independent

function test_if_else_same_name()
    local outer = 1
    if true then
        local x = 10
        printh(x)  -- 10
    else
        local x = 20  -- different x, should not conflict
        printh(x)  -- won't execute
    end
    -- x is not accessible here
end

function test_if_else_outer_access()
    local a = 100
    if true then
        local a = 200  -- shadows outer a
        printh(a)      -- 200
    else
        local a = 300  -- different a
        printh(a)      -- won't execute
    end
    printh(a)  -- 100 (outer a)
end

function test_nested_if()
    local val = 1
    if true then
        local val = 2
        printh(val)  -- 2
        if true then
            local val = 3
            printh(val)  -- 3
        end
        printh(val)  -- 2
    end
    printh(val)  -- 1
end

function test_elseif_scoping()
    local x = 0
    if false then
        local x = 1
        printh(x)
    elseif false then
        local x = 2
        printh(x)
    elseif true then
        local x = 3
        printh(x)  -- 3
    else
        local x = 4
        printh(x)
    end
    printh(x)  -- 0
end

function test_while_scoping()
    local i = 0
    while i < 3 do
        local x = i * 10
        printh(x)
        i = i + 1
    end
    -- x is not accessible here
    printh(i)  -- 3
end

function main()
    test_if_else_same_name()
    printh("---")
    test_if_else_outer_access()
    printh("---")
    test_nested_if()
    printh("---")
    test_elseif_scoping()
    printh("---")
    test_while_scoping()
    return 0
end
