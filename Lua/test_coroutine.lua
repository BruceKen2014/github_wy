

local ui_thread = coroutine.create(
	function()
		local count = 0
		while(true) do
			print("ui thread render somthing!", count)
			count = count + 1
			coroutine.yield()
		end
	end
)

local function test_coroutine()
	print("first")
	coroutine.resume(ui_thread)
	print("two")
	coroutine.resume(ui_thread)
	print("three")
	coroutine.resume(ui_thread)
end

