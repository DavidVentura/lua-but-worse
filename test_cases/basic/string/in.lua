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
	test_sub()
	--test_concat()
end
