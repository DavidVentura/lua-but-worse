-- this being global is important for the test
-- an anonymous function that is not-inside-a-function should work
-- an anonymous-table in an anonymous-function in a global-anonymous-table should work

this = {}
this.attr = {key="value_top"}
func_in_tab = {
	func=function(this)
		this.attr = {key="value_attr"}
		printh("func in tab")
	end,
}
function main()
	literals = {
		["top"] = {x=0, y=-68},
		["bottom"] = {x=0, y=68},
		["left"] = {x=-68, y=0},
		["right"] = {x=68, y=0}
	}
	printh(literals.top.y)
	printh(literals.right.x)

	func_in_tab.func(func_in_tab)
	return 0
end
