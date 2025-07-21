上级目录：[[任务系统]]
用于新建一个对象

示例：
```cpp
auto currentPilot = SolarSystemMgr::getInstance().currentPilot;
UINT ContainerID = currentPilot->currentShip->GetComponent<CargoContainerComponent>()->containerID;
std::shared_ptr<Task> pTask = std::make_shared<Task>();
pTask->isInnerTask = true;
pTask->publisher = sourceObject;
pTask->target = targetObject;
pTask->targetSystem = SOLAR_SYSTEM;
(*pTask->paramsPtr)["handlerType"] = std::string("transferObject");
(*pTask->paramsPtr)["ContainerID"] = static_cast<int>(ContainerID);
TaskMgr::getInstance().addTask(pTask);
```


| 参数名         | 类型     | 描述     |
| ----------- | ------ | ------ |
| handlerType | string |        |
| ContainerID | int    | 目标容器id |

