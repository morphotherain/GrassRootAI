#include "SpaceTransformComponent.h"
#include "dynGameObjectsManager.h"
#include "mapDenormalizeManager.h"

SpaceTransformComponent::SpaceTransformComponent(UINT _objectID)
{
	orientation = { 0.0f, 0.0f, 0.0f, 0.0f };
	if(_objectID<40000000)
	{
		auto pos = dynGameObjectsManager::getInstance()->getPosByObjectID(_objectID);
		auto q = dynGameObjectsManager::getInstance()->getQuaternionByObjectID(_objectID);
		x = pos[0];
		y = pos[1];
		z = pos[2];
		orientation.w = static_cast<float>(q[0]);
		orientation.x = static_cast<float>(q[1]);
		orientation.y = static_cast<float>(q[2]);
		orientation.z = static_cast<float>(q[3]);
		objectID = _objectID;
	}
	else {
		auto pos = mapDenormalizeManager::getInstance()->getPosByObjectID(_objectID);
		x = pos[0];
		y = pos[1];
		z = pos[2];
		objectID = _objectID;
	}

}

void SpaceTransformComponent::store()
{
	std::vector<double> pos = { x, y, z };  // 将当前组件的位置信息封装到vector中
	// 通过单例模式获取dynGameObjectsManager实例，调用对应的更新函数来更新位置和四元数信息
	dynGameObjectsManager* manager = dynGameObjectsManager::getInstance();
	if (manager != nullptr)
	{
		manager->updatePosByObjectID(objectID, pos);
	}
}
