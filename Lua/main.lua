

package.loaded.base = nil
package.loaded.pre_define = nil
package.loaded.animal = nil
package.loaded.vector = nil
package.loaded.array = nil
package.loaded.test_coroutine = nil

local base_define = require "base"
local pre_define = require "base.pre_define"
local animal_cls = require "animal"
local vector = require "vector"
local array = require "array"
local test_coroutine = require "test_coroutine"




local ftable = { name = "new table"}
setmetatable(ftable, {__gc = function(t) print(t.name, " is gc") end})
ftable = nil
collectgarbage()


--模拟表格key的查找
local Base1 = { a = 10}
local Base2 = { a = 10}
Base2.__index = Base2

local DerivedTable = {}
local DerivedTable2 = {}

setmetatable(DerivedTable2,Base1)

print(Base2.a)
print(DerivedTable.a)
print(DerivedTable2.a)