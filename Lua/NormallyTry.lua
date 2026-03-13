



local t = {a=1,b=2}
local c = {}
setmetatable(c, { __index = t,__newindex = t})

print(c.a)
c.a = 3
c.x = 1
print(t.x)
