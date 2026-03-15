

--测试工程的基础class

--函数 Class，用于创建新类（可继承自已有的基类）
local function Class(base)
    -- 创建一个新的类表，它将作为其实例的元表
    local cls = {}
    -- 设置 __index 指向自身，这样实例在查找字段时，如果自身没有，就会去 cls 中找
    cls.__index = function(t, k) 
		--print(tostring(t), " rawget ", k , "   ", rawget(t,k))
		return rawget(cls,k)
	end
		
	cls.__newindex = function(t, k, v) 
		--print(tostring(t), " set " .. k .. " = " .. tostring(v) )
		rawset(t, k, v)
	end
	
    -- 如果指定了基类，则设置 cls 的元表，使 cls 能够继承基类的方法
    setmetatable(cls, base or cls)

    -- 类的构造函数，用于创建实例
    function cls:new(...)
		--print("cls:new ,self is ", tostring(self))
        -- 创建一个新表，并将其元表设为当前类（cls）
        local obj = setmetatable({}, self)
        -- 如果类中定义了 __init 方法，则调用它进行初始化
        if obj.init then
            obj:init(...)
        end
        return obj
    end
	
	cls.__call = function(_, ...)
			--print("cls.__call cls is ", tostring(cls))
            return cls:new(...)   -- 调用类的 new 方法
        end

    return cls
end

return 

