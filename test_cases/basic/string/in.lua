function test_empty_thing()
  local an_empty_one = ""
  local another_empty_one = ""

  local n1 = 5
  local n2 = 6
  local a = an_empty_one..n1
  local b = an_empty_one..n2
end
function test_sub()
  local s="something"
  printh("test_sub")
  printh(sub(s, 5))     -- thing
  printh(sub(s,-5))     -- thing
  printh(sub(s,-4,-3))  -- hi
  printh(sub(s,-2,nil)) -- n
  printh(sub(s, 8,nil)) -- n

end
function test_concat()
  printh("a")
  printh("a".."b")
  local ones = "11111111111111111111"
  local twos = "22222222222222222222"
  printh(ones..twos)
  printh("a"..5)
  printh(""..5)
end
function main()
	test_empty_thing()
	test_sub()
	test_concat()
end
