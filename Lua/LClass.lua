
LClass函数分析
function LClass(SuperClass)

--最终要返回的类，先赋值为1个空table
local newClass = {}

--设置元表
--这里元表的__index是一个函数
  --这里有个问题，因为只支持单继承，直接把__index=SuperClass不就行了吗，为什么要把__index设置为一个function呢？
--function(t,k)里面除了自下而上查找成员外，还进行了一个rawset操作，意思是找到后立马设置设置到本table里面，这个改进是为了提效？
  setmetatable(newClass, {
    __index = function(t, k)
      local super = t --这里直接把super设置为t，似乎非必要？第一次的查找肯定会失败，如果如果真的在t中，就不会进入到__index里面了
      while super do
        local v = rawget(super, k)
        if v ~= nil then
		  --这里找到之后rawset本table，似乎可以在继承体系的查找中提效
          rawset(t, k, v)
          return v
        end
        super = rawget(super, "Super")
      end
	  --既然整个体系都没找到，何必再在这里rawget一次？岂不是多此一举？直接return nil不就得了？
      local p = rawget(t, k)
      if p ~= nil then
        rawset(t, k, p)
      end
      return p
    end,

--这里接管了__newindex，意思是在设置变量的时候，直接在本table里面进行设置
--这TM不是脱裤子放屁吗？这和不设置__newindex有啥区别？因为不设置__newindex，在设置的时候就是设置在t里面的啊
    __newindex = function(t, k, v)
      rawset(t, k, v)
    end
  })

--设置本类的父类，如果SuperClass为nil，则说明没有父类
--需要注意，这里只支持了单继承，似乎没有支持多继承
newClass.Super = SuperClass

--设置构造函数和析构函数的字段为false
  newClass.Constructor = false
  newClass.Destructor = false

--添加New函数，new函数负责返回一个类的实例
  newClass.New = function(...)
--添加一个待返回的实例变量，并且设置变量的ClassType为本类
--相当于添加table指针，指向本table
    local Obj = {ClassType = newClass}

--设置实例的元表，并且设置元表的__index 为本table
--这将实例与本table建立了联系
    setmetatable(Obj, {__index = newClass})

--到这里，还缺少一个东西？要构造一个类的实例，得先构造这个类的父类部分！
--于是下面代码就做这个工作，外围这个do end只是为了强调Create(newClass, ...)使用的Create是局部的Create的，没什么具体意义
--
    do
      local Create
      Create = function(C, ...)
--这里加入的是递归调用，只要class有父类就一直上溯，但到这里还没有做什么事情
        if C.Super then
          Create(C.Super, ...)
        end
--上溯完成，判断class是否有构造函数，如果有，则调用构造函数
--这里的一个关键是，使用.点调用，而不是冒号调用，这意味着self必须手动传入
--而函数调用的时候也的确把实例Obj传入了进去
        if C.Constructor then
          C.Constructor(Obj, ...)
        end
      end
      Create(newClass, ...)
    end
    return Obj
  end

--添加Delete函数，内部处理逻辑和New类似
  newClass.Delete = function(Obj)
    do
      local Destroy
      Destroy = function(C)
--这里和Create不一样的是，先析构自身，再析构父类
        if C.Destructor then
          C.Destructor(Obj)
        end
        if C.Super then
          Destroy(C.Super)
        end
      end

      Destroy(newClass)
    end
  end

  return newClass
end