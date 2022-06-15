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
        return 0
end
