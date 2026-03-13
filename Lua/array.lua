--[[
lua中实现简单的数组,相当于在C++中实现vector
数组功能有：Add,Remove,RemoveAt,Clear,Length
--]]


local TArray = {}

TArray.__index = TArray
setmetatable(TArray, TArray)

function TArray:Create(...)
	local ret = {}
	setmetatable(ret, self)
	ret:Add(...)
	return ret
end


--获取指定位置的元素
function TArray:Get(index)
	--self本身就是一个table，而table天然支持[]取值
	return self[index]
end

--获取数组长度
function TArray:Length()
	return #self
end

--添加数组元素
function TArray:Add(...)
	local temp = {...}
	for k,v in ipairs(temp) do 
		table.insert(self,v)
	end
end

--删除数组元素
function TArray:Remove(element)
	for i = 1, self:Length() do
		if(self:Get(i) == element)then
			self:RemoveAt(i)
			return true
		end
	end
	return false
end

--删除指定位置的元素
function TArray:RemoveAt(index)
	table.remove(self,index)
end

--清空元素
function TArray:Clear()
	while(self:Length() ~= 0) do
		self:RemoveAt(1)
	end
end

-------------------操作符重载------------------------
--重载__call,模拟函数式调用
function TArray.__call(self,...)
	local ret = self:Create(...)
	return ret
end

--[[重载#  这里的TArray有点特别，因为TArray本身就只用table作为存储，没有额外的数据
--所以不用重载#，如果是一些特别的class，可以重载#
function TArray.__len(arr)
	return arr:Length()
end
--]]
---[[重载__add
function TArray.__add(arr1,arr2)
	local ret = TArray()
	
	for i=1,arr1:Length() do
		table.insert(ret,arr1:Get(i))
	end
	
	for i=1,arr2:Length() do
		table.insert(ret,arr2:Get(i))
	end
	
	return ret
end
--]]
---[[重载__div
function TArray.__div(arr1,v)
	local ret = TArray()
	for i=1,arr1:Length() do
		table.insert(ret,arr1:Get(i) / v)
	end
	return ret
end
--]]
---[[重载tostring操作符
function TArray.__tostring(arr)
	local str = ""
	for i=1,arr:Length() do
		str = str .. " " ..tostring(arr:Get(i))
	end
	return str
end
--]]
function test_TArray()
	local arr1 = TArray()
	print(arr1)
	
	arr1:Add(1,2,3,10)
	print("length," , #arr1)
	
	print(arr1)
	arr1:RemoveAt(2)
	print(arr1)
	arr1:Remove(10)
	print(arr1)
	
	local arr2 = TArray(10,13,12)
	print(arr2)
	local arr3 = arr1 + arr2
	print(arr3)
	
	local a4 = arr3 / 3
	print(a4)
	a4:Clear()
	print(a4)
end

--test_TArray()

