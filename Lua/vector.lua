--[[
测试lua中的操作符重载
操作符重载的执行步骤：
遇见v1 + v2，
1、判断v1是一个表，并且进行了+操作，于是问：v1有没有元表？
2、查知v1元表是FVector，于是问:FVector有没有__index？
3、查知有__index，并且是自身，于是问:FVector有没有实现__add操作符？
4、查知FVector实现了__add操作符，于是把+号两边的变量作为参数，传给__add
5、将__add返回的值作为v1+v2的返回值
6、操作结束
--]]
local FVector = 
{ 
x = 0, 
y = 0, 
z = 0
}

function FVector:New(...)
	local ret = {}
	self.__index = self
	setmetatable(ret, self)
	
	return ret
end

FVector.__index = FVector
setmetatable(FVector, FVector)

function FVector.__tostring(self)
	local ret = "x: " .. self.x .. " y: " .. self.y .. " z: " .. self.z
	return ret
end

function FVector.__call(self,...)
	local ret = self:New()
	return ret
end

--operator +
function FVector.__add(v1,v2)
	local ret = FVector()
	ret.x = v1.x + v2.x 
	ret.y = v1.y + v2.y
	ret.z = v1.z + v2.z
	return ret
end
--operator -
function FVector.__sub(v1,v2)
	local ret = FVector()
	ret.x = v1.x - v2.x 
	ret.y = v1.y - v2.y
	ret.z = v1.z - v2.z
	return ret
end
--operator *
function FVector.__mul(v1,v2)
	local ret = FVector()
	ret.x = v1.x * v2.x 
	ret.y = v1.y * v2.y
	ret.z = v1.z * v2.z
	return ret
end

function test_FVector()
	local v1 = FVector()
	local v2 = FVector()
	v1.x = 10
	v2.x = 20
	print(v1 + v2)
	print(v1 - v2)
	print(v1 * v2)
end

--test_FVector()
