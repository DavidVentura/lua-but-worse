function main()
        captured = 7
        a = function(x)
                return x*captured
        end
        print(a(5))

        b = function(x, y)
                return x*y
        end
        print(b(5, 6))

	c = {}
	c['f'] = function()
                return "works inside a table"
        end
	print(c.f())

	v = 'index'
	c[v] = function()
                return "works inside a table, via hashmap"
        end
	print(c[v]())

        return 0
end
