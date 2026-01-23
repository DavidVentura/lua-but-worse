from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from capture_detector import CaptureDetector
from ast_nodes import *


def parse_and_analyze(code):
    """Helper to parse code and run scope resolution + capture detection"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    return scopes, global_scope


def test_simple_capture():
    code = """
local x = 1
function outer()
    return x
end
"""
    scopes, global_scope = parse_and_analyze(code)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    outer_scope = scopes[1]
    assert x_var in outer_scope.captures


def test_no_capture_same_scope():
    code = """
function foo()
    local x = 1
    return x
end
"""
    scopes, global_scope = parse_and_analyze(code)

    func_scope = scopes[1]
    x_var = func_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL
    assert len(func_scope.captures) == 0


def test_nested_capture():
    code = """
local x = 1
function outer()
    local y = 2
    function inner()
        return x + y
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    outer_scope = scopes[1]
    y_var = outer_scope.vars['y']
    assert y_var.kind == VarKind.LOCAL

    inner_scope = scopes[2]
    assert x_var in inner_scope.captures
    assert y_var in inner_scope.captures


def test_parameter_capture():
    code = """
function outer(a, b)
    function inner()
        return a + b
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    outer_scope = scopes[1]
    a_var = outer_scope.vars['a']
    b_var = outer_scope.vars['b']

    assert a_var.kind == VarKind.PARAM
    assert b_var.kind == VarKind.PARAM

    inner_scope = scopes[2]
    assert a_var in inner_scope.captures
    assert b_var in inner_scope.captures


def test_anonymous_function_capture():
    code = """
local x = 1
local f = function()
    return x
end
"""
    scopes, global_scope = parse_and_analyze(code)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    anon_scope = scopes[1]
    assert x_var in anon_scope.captures


def test_capture_in_loop():
    code = """
local x = 1
for i=1,10 do
    function foo()
        return x + i
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    loop_scope = scopes[1]
    i_var = loop_scope.vars['i']
    assert i_var.kind == VarKind.LOCAL

    func_scope = scopes[2]
    assert x_var in func_scope.captures
    assert i_var in func_scope.captures


def test_multiple_functions_same_capture():
    code = """
local x = 1
function f1()
    return x
end
function f2()
    return x * 2
end
"""
    scopes, global_scope = parse_and_analyze(code)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    f1_scope = scopes[1]
    f2_scope = scopes[2]
    assert x_var in f1_scope.captures
    assert x_var in f2_scope.captures


def test_deep_nesting():
    code = """
local a = 1
function f1()
    local b = 2
    function f2()
        local c = 3
        function f3()
            return a + b + c
        end
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    a_var = global_scope.vars['a']
    f1_scope = scopes[1]
    b_var = f1_scope.vars['b']
    f2_scope = scopes[2]
    c_var = f2_scope.vars['c']
    f3_scope = scopes[3]

    assert a_var.kind == VarKind.LOCAL
    assert b_var.kind == VarKind.LOCAL
    assert c_var.kind == VarKind.LOCAL

    assert a_var in f3_scope.captures
    assert b_var in f3_scope.captures
    assert c_var in f3_scope.captures


def test_no_capture_loop_variable_in_loop():
    code = """
for i=1,10 do
    print(i)
end
"""
    scopes, global_scope = parse_and_analyze(code)

    loop_scope = scopes[1]
    i_var = loop_scope.vars['i']
    assert i_var.kind == VarKind.LOCAL


def test_global_not_captured():
    code = """
x = 1
function foo()
    return x
end
"""
    scopes, global_scope = parse_and_analyze(code)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.GLOBAL

    func_scope = scopes[1]
    assert len(func_scope.captures) == 0


def test_function_call_as_statement():
    code = """
local x = 1
function foo()
    print(x)
end
"""
    scopes, global_scope = parse_and_analyze(code)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    func_scope = scopes[1]
    assert x_var in func_scope.captures


def test_method_call_as_statement():
    code = """
local x = 1
function foo()
    obj:method(x)
end
"""
    scopes, global_scope = parse_and_analyze(code)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    func_scope = scopes[1]
    assert x_var in func_scope.captures


def test_global_function_not_captured():
    """Test that global functions are not marked as captured when called from other functions"""
    code = """
function something(arg)
    return arg
end

function main()
    something(5)
end
"""
    scopes, global_scope = parse_and_analyze(code)

    # something is defined in global scope
    something_var = global_scope.vars['something']

    # main is also in global scope
    main_scope = scopes[2]  # scope 0=global, 1=something, 2=main

    # something should NOT be captured by main
    assert something_var not in main_scope.captures, f"Global function 'something' should not be captured, but main.captures = {main_scope.captures}"


def test_closure_basic_shared_mutable_state():
    """Test basic closure with shared mutable state (test_cases/compound/closure/in.lua)"""
    code = """
function main()
  local captured = 7
  a = function(x)
    return x*captured
  end
  printh(a(5))

  captured = 1
  printh(a(5))
  function b()
    captured = 2
  end
  b()
  printh(a(5))

  return 0
end
"""
    scopes, global_scope = parse_and_analyze(code)

    main_scope = scopes[1]
    captured_var = main_scope.vars['captured']

    # Both anonymous function 'a' and function 'b' should capture 'captured'
    anon_a_scope = scopes[2]
    func_b_scope = scopes[3]

    assert captured_var in anon_a_scope.captures, "Anonymous function 'a' should capture 'captured'"
    assert captured_var in func_b_scope.captures, "Function 'b' should capture 'captured'"


def test_closure_double_nested_forloop():
    """Test capturing loop variables from double nested for loops (test_cases/compound/closure/1.lua)"""
    code = """
function test_double_enclosing_fornum_iterator()
    printh("double_enclosing fornum iterator")
    for x=1,2 do
        for y=1,2 do
            function whatever_func()
                printh(x*y)
            end
            whatever_func()
        end
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    # Find the loop scopes and function scope
    outer_loop_scope = scopes[2]  # for x
    inner_loop_scope = scopes[3]  # for y
    func_scope = scopes[4]  # whatever_func

    x_var = outer_loop_scope.vars['x']
    y_var = inner_loop_scope.vars['y']

    assert x_var in func_scope.captures, "Function should capture outer loop variable 'x'"
    assert y_var in func_scope.captures, "Function should capture inner loop variable 'y'"


def test_closure_single_forloop():
    """Test capturing loop variable from single for loop (test_cases/compound/closure/2.lua)"""
    code = """
function test_enclosing_fornum_iterator()
    printh("enclosing fornum iterator")
    for i=1,2 do
        function func()
            printh(i)
        end
        func()
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    loop_scope = scopes[2]
    func_scope = scopes[3]
    i_var = loop_scope.vars['i']

    assert i_var in func_scope.captures, "Function should capture loop variable 'i'"


def test_closure_table_field_vs_captured():
    """Test that table fields are not confused with captured variables (test_cases/compound/closure/3.lua)"""
    code = """
function test_enclosing_table_index()
    printh("enclosing arg overlap")
    function wrapper(x,y)
        local obj = {}
        obj.x = "not wrapped"
        obj.a = "wrapped"
        function wrapped()
            printh(obj.x)
            printh(obj[y])
        end
        wrapped()
    end
    wrapper(5, "a")
end
"""
    scopes, global_scope = parse_and_analyze(code)

    wrapper_scope = scopes[2]
    wrapped_scope = scopes[3]

    x_var = wrapper_scope.vars['x']
    y_var = wrapper_scope.vars['y']
    obj_var = wrapper_scope.vars['obj']

    # 'wrapped' should capture 'obj' and 'y', but NOT 'x' (obj.x is a table field access)
    assert obj_var in wrapped_scope.captures, "Function should capture 'obj'"
    assert y_var in wrapped_scope.captures, "Function should capture 'y'"
    assert x_var not in wrapped_scope.captures, "Function should NOT capture 'x' (only used as obj.x)"


def test_closure_returning_closure():
    """Test returning closures with independent state (test_cases/compound/closure/4.lua)"""
    code = """
function newCounter ()
  local i = 0
  return function ()
    i = i + 1
    return i
  end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    newCounter_scope = scopes[1]
    anon_func_scope = scopes[2]

    i_var = newCounter_scope.vars['i']

    assert i_var in anon_func_scope.captures, "Returned anonymous function should capture 'i'"


def test_closure_function_args_captured():
    """Test capturing function arguments (test_cases/compound/closure/5.lua)"""
    code = """
function f(arg)
    function f2()
        printh(arg)
    end
    return f2
end
"""
    scopes, global_scope = parse_and_analyze(code)

    f_scope = scopes[1]
    f2_scope = scopes[2]

    arg_var = f_scope.vars['arg']

    assert arg_var.kind == VarKind.PARAM, "'arg' should be a parameter"
    assert arg_var in f2_scope.captures, "Nested function 'f2' should capture parameter 'arg'"


def test_multiple_parameters_captured():
    """Test capturing multiple parameters"""
    code = """
function outer(a, b, c)
    function inner()
        return a + b + c
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    outer_scope = scopes[1]
    inner_scope = scopes[2]

    a_var = outer_scope.vars['a']
    b_var = outer_scope.vars['b']
    c_var = outer_scope.vars['c']

    assert a_var.kind == VarKind.PARAM
    assert b_var.kind == VarKind.PARAM
    assert c_var.kind == VarKind.PARAM

    assert a_var in inner_scope.captures, "Inner should capture parameter 'a'"
    assert b_var in inner_scope.captures, "Inner should capture parameter 'b'"
    assert c_var in inner_scope.captures, "Inner should capture parameter 'c'"


def test_parameter_and_local_captured():
    """Test capturing both parameters and local variables"""
    code = """
function outer(param1, param2)
    local local1 = 1
    local local2 = 2
    function inner()
        return param1 + local1
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    outer_scope = scopes[1]
    inner_scope = scopes[2]

    param1_var = outer_scope.vars['param1']
    param2_var = outer_scope.vars['param2']
    local1_var = outer_scope.vars['local1']
    local2_var = outer_scope.vars['local2']

    assert param1_var.kind == VarKind.PARAM
    assert local1_var.kind == VarKind.LOCAL

    assert param1_var in inner_scope.captures, "Should capture parameter 'param1'"
    assert local1_var in inner_scope.captures, "Should capture local 'local1'"
    assert param2_var not in inner_scope.captures, "Should NOT capture unused parameter 'param2'"
    assert local2_var not in inner_scope.captures, "Should NOT capture unused local 'local2'"


def test_parameter_modified_and_captured():
    """Test capturing parameters that are modified"""
    code = """
function counter(start)
    start = start or 0
    function increment()
        start = start + 1
        return start
    end
    return increment
end
"""
    scopes, global_scope = parse_and_analyze(code)

    counter_scope = scopes[1]
    increment_scope = scopes[2]

    start_var = counter_scope.vars['start']

    assert start_var.kind == VarKind.PARAM
    assert start_var in increment_scope.captures, "Should capture and modify parameter 'start'"


def test_anonymous_function_captures_parameter():
    """Test anonymous function capturing parameter"""
    code = """
function make_adder(x)
    return function(y)
        return x + y
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    make_adder_scope = scopes[1]
    anon_scope = scopes[2]

    x_var = make_adder_scope.vars['x']
    y_var = anon_scope.vars['y']

    assert x_var.kind == VarKind.PARAM, "'x' should be a parameter of make_adder"
    assert y_var.kind == VarKind.PARAM, "'y' should be a parameter of anonymous function"
    assert x_var in anon_scope.captures, "Anonymous function should capture outer parameter 'x'"
    assert y_var not in anon_scope.captures, "Anonymous function should NOT capture its own parameter 'y'"


def test_deeply_nested_parameter_capture():
    """Test parameter captured by deeply nested function"""
    code = """
function level1(a)
    function level2(b)
        function level3(c)
            function level4()
                return a + b + c
            end
        end
    end
end
"""
    scopes, global_scope = parse_and_analyze(code)

    level1_scope = scopes[1]
    level2_scope = scopes[2]
    level3_scope = scopes[3]
    level4_scope = scopes[4]

    a_var = level1_scope.vars['a']
    b_var = level2_scope.vars['b']
    c_var = level3_scope.vars['c']

    # level4 should capture all three parameters from outer functions
    assert a_var in level4_scope.captures, "Innermost should capture 'a' from level1"
    assert b_var in level4_scope.captures, "Innermost should capture 'b' from level2"
    assert c_var in level4_scope.captures, "Innermost should capture 'c' from level3"

    # Intermediate levels shouldn't capture what they don't use
    assert len(level3_scope.captures) == 0, "level3 doesn't reference outer parameters"
    assert len(level2_scope.captures) == 0, "level2 doesn't reference outer parameters"


def test_parameter_shadowing():
    """Test that parameter shadowing works correctly"""
    code = """
function outer(x)
    function inner(x)
        return x
    end
    return outer_x
end
"""
    scopes, global_scope = parse_and_analyze(code)

    outer_scope = scopes[1]
    inner_scope = scopes[2]

    outer_x = outer_scope.vars['x']
    inner_x = inner_scope.vars['x']

    assert outer_x.kind == VarKind.PARAM
    assert inner_x.kind == VarKind.PARAM
    assert outer_x != inner_x, "Parameters should be different variables"
    assert outer_x not in inner_scope.captures, "Shadowed parameter should not be captured"
