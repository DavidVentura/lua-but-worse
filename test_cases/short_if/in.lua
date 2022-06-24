function main()
  test = false
  if (test) return 5
  if (test) return 5 else a = 1
  if (test) return 5 else b = 2 c = 3
  if (not test) d = -4 else e = 0
  print(a)
  print(b)
  print(c)
  print(d)

if (test) then
    print("this should not be visible")
end
return 0
end
