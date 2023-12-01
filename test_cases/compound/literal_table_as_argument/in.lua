particles = {}
function add_particle(x, y)
	add(particles, {
		x = x,
		y = y,
	})
end

function main()
	add_particle(1, 5)
	foreach(particles, function(particle)
		printh(particle.x)
		printh(particle.y)
	end)
end
