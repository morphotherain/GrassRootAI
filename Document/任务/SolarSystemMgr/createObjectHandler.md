上级目录：[[SolarSystemManager - Handler]]
用于新建一个对象

示例（推荐）：
```cpp
CreateObjectParams params;
params.typeID = static_cast<int>(m_pBase->typeID);
params.ownerID = 0;
params.containerID = static_cast<int>(containerID);
params.attributes = std::vector<Attribute>{ { ATTR_ID_QUANTITY, std::floor(1) } };

auto pTask = TaskFactory::MakeCreateObjectTask(nullptr, params);
pTask->isInnerTask = true;
pTask->publisherId = objectID;
```

旧写法（仍兼容，paramsPtr key 不变）：
```cpp
std::shared_ptr<Task> pTask = std::make_shared<Task>();
pTask->isInnerTask = true;
pTask->publisherId = objectID;
pTask->targetSystem = SOLAR_SYSTEM;
(*pTask->paramsPtr)["handlerType"] = std::string("createObject");
(*pTask->paramsPtr)["typeID"] = static_cast<int>(m_pBase->typeID);
(*pTask->paramsPtr)["OwnerID"] = 0;
(*pTask->paramsPtr)["ContainerID"] = static_cast<int>(containerID);
std::vector<Attribute> attributes = {
	{ATTR_ID_QUANTITY, std::floor(1) },
};
(*pTask->paramsPtr)["attributes"] = attributes;
```

详见 [[TaskParams]]。


| 参数名         | 类型                       | 描述  |
| ----------- | ------------------------ | --- |
| handlerType | string                   |     |
| typeID      | int                      |     |
| OwnerID     | int                      |     |
| ContainerID | int                      |     |
| attributes  | std::vector\<Attribute\> |     |
