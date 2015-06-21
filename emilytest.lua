
function new(ui, node_def)

	local newnode = 5

	for _,v in ipairs(node_def) do

		local sub = nil
		if type(v) == "table" then

			sub = new(ui, v)
		end

		newnode:addchild(sub)
	end
end

