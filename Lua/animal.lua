
--[[
本文件模拟了lua的面向对象编程
1、类的模拟
2、类的实例化
3、类的继承
4、子类覆写父类的成员方法
5、多态调用
--]]


--====================base class animal=====================
--模拟类的定义
local animal = {}
animal.name = "none"
animal.weight = 0
animal.age = 0

----模拟类的实例化，本质上是创建一个新表，新表的元表是旧表，旧表的__index是旧表自己
function animal:New()
	self.__index = self
	local instance = {}
	setmetatable(instance,self)
	return instance
end
--模拟类的成员函数
function animal:Log()
	print("animal info:")
	print("		name: ", self.name)
	print("		age: ", self.age)
	print("		weight, ", self.weight)
end

--模拟animal()这种函数式调用
function animal.__call(self,...)
	local ret = self:New()
	return ret
end

--模拟多态，因为lua的弱类型，天然就支持多态
local function IntroduceAnima(anim)
	anim:Log()
end

--=====================derived class============================
--模拟类继承，本质上是生成一个新表，新表的元表是参数表，并把参数表的__index设置成参数表自身
--这里还作了一个小改进，即把新表的Super设置为旧表，除了这点外，和类的实例化没有区别
local function Inherit(cls)
	cls.__index = cls
	local child = { Super = cls}
	setmetatable(child,cls)
	return child
end
--类继承
local bird = Inherit(animal)
bird.name = "bird!"
bird.fly_time = 0

--继承类添加新函数
function bird:FlyBegin()
	print("lets begin fly!", self.name)
end

--继承类override父类函数
function bird:Log()
	--这里为什么不用self.Super:Log()？因为这会导致Log接收的实际对象变为animal，导致Log输出的目标不对，实际Log这里接收的指针应该是bird
	self.Super.Log(self)
	print("		fly_time, ", self.fly_time)
end

--类继承
local beast = Inherit(animal)
--继承类自己添加成员变量
beast.name = "beast!"
beast.legs = 4

--继承类添加新函数
function beast:RunBegin()
	print("lets begin run!", self.name)
end

--继承类override父类函数
function beast:Log()
	self.Super.Log(self)
	print("		legs, ", self.legs)
end

function test_animal()
	--类的实例化
	local bird1 = bird()
	bird1.name = "flying bird number one!"
	IntroduceAnima(bird1)

	local beast1 = beast()
	beast1.name = "beast tiger!"
	IntroduceAnima(beast1)
end

local animal_cls = {animal = animal,bird = bird}

return animal_cls 