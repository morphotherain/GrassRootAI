#include "SolarSystem.h"
#include "InvTypesManager.h"
#include "invGroupsManager.h"
#include "eveBracketsManager.h"
#include "ObjectFactory.h"
#include "mapDenormalizeManager.h"

void SolarSystem::Init()
{
	getDenormalizesBySolarSystemID();
	auto p_vGameObjectData = dynGameObjectsManager::getInstance()->getGameObjectBySolarSystemID(m_solarSystem.solarSystemID);
	for (auto& objectData : *p_vGameObjectData) {
		addGameObject(objectData);
	}
	for (auto p_object : space_objects) {
		auto p = p_object.lock();
		if(p)
			addObjectToSector(p);
	}
	/*for (auto p_object : other_objects) {
	}*/
}

void SolarSystem::Update(UINT tick)
{
	auto it = std::remove_if(space_objects.begin(), space_objects.end(),
		[](const std::weak_ptr<GameObject>& wp) {
			return wp.expired();
		});
	space_objects.erase(it, space_objects.end());
	for (auto obj : space_objects) {
		auto p = obj.lock();
		if (p)
			p->Update(tick);
	}

	if (tick % 30 == 0) {
		checkObjectsInSector();
		setCurrentSector();
	}

	//Tran->x -= 16;
}

void SolarSystem::getDenormalizesBySolarSystemID()
{
	auto rows = mapDenormalizeManager::getInstance()->queryStaticObjectsBySolarSystemID(
		static_cast<int>(m_solarSystem.solarSystemID));

	for (auto& row : rows)
	{
		auto p_denormalize = std::make_shared<DenormalizeData>(row.denormalize);
		m_denormalizes.push_back(p_denormalize);
		addGameObject(row.gameObject);
	}

	for (auto p_denormalize : m_denormalizes)
	{
		p_denormalize->groupID = InvTypesManager::getInstance()->getGroupByTypeId(p_denormalize->typeID);
		p_denormalize->bracketID = invGroupsManager::getInstance()->getBracketIDByGroupId(p_denormalize->groupID);
		p_denormalize->dds_path = eveBracketsManager::getInstance()->getPathByTypeId(p_denormalize->bracketID);
	}
}

void SolarSystem::addGameObject(dynGameObject& objectData)
{
	if (p_mapObject->find(objectData.objectID) != p_mapObject->end())
	{
		return;
	}

	auto object = ObjectFactory::CreateFromDynObject(objectData);
	if (!object)
	{
		return;
	}

	(*p_mapObject)[objectData.objectID] = object;
	object->objectID = objectData.objectID;
	if (object->GetComponent<SpaceTransformComponent>() != nullptr)
	{
		space_objects.push_back(object);
	}
	if (object->GetComponent<BaseComponent>() != nullptr)
	{
		auto base = object->GetComponent<BaseComponent>();
		base->objectID = objectData.objectID;
		base->typeID = objectData.typeID;
		base->ownerID = objectData.OwnerID;
		base->containerID = objectData.ContainerID;
		base->solarSystemID = objectData.SolarSystemID;
		base->groupID = objectData.groupID;
		base->categoryID = objectData.categoryID;
		if (objectData.name != L"" && base->name == L"")
		{
			base->name = objectData.name;
		}
	}
}

long long int SolarSystem::CalculateHashIndex(double x, double y, double z)
{
	return 0;
}

void SolarSystem::addObjectToSector(std::shared_ptr<GameObject> object) {
	auto Tran = object->GetComponent<SpaceTransformComponent>();

	auto sector = getSector(Tran->x, Tran->y, Tran->z);
	if (sector == nullptr) {
		sector = addSector(Tran->x, Tran->y, Tran->z);
		sector->addObject(object);
	}
	else {
		sector->addObject(object);
	}
}

std::shared_ptr<Sector> SolarSystem::addSector(double x, double y, double z) {
	std::shared_ptr<Sector> newSector = std::make_shared<Sector>();

	const long long int gridSideLength = 10000000;
	long long int xInt = static_cast<long long int>(x / gridSideLength);
	long long int yInt = static_cast<long long int>(y / gridSideLength);
	long long int zInt = static_cast<long long int>(z / gridSideLength);
	// 对取整后的负数坐标进行调整
	if (xInt < 0) xInt -= 1;
	if (yInt < 0) yInt -= 1;
	if (zInt < 0) zInt -= 1;

	// 分别根据处理后的x、y、z坐标作为各层哈希表的键
	std::unordered_map<long long int, std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>>::iterator outerIt;
	std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>::iterator middleIt;
	std::unordered_map<long long int, std::shared_ptr<Sector>>::iterator innerIt;

	// 最外层哈希表查找或插入
	outerIt = m_Sectors.find(xInt);
	if (outerIt == m_Sectors.end()) {
		// 如果不存在，插入一个新的中层哈希表
		std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>> newMiddleMap;
		m_Sectors[xInt] = newMiddleMap;
		outerIt = m_Sectors.find(xInt);
	}

	// 中层哈希表查找或插入
	middleIt = outerIt->second.find(yInt);
	if (middleIt == outerIt->second.end()) {
		// 如果不存在，插入一个新的内层哈希表
		std::unordered_map<long long int, std::shared_ptr<Sector>> newInnerMap;
		outerIt->second[yInt] = newInnerMap;
		middleIt = outerIt->second.find(yInt);
	}

	// 内层哈希表查找或插入
	innerIt = middleIt->second.find(zInt);
	if (innerIt == middleIt->second.end()) {
		double xFull = static_cast<double>(xInt * 10000000);
		double yFull = static_cast<double>(yInt * 10000000);
		double zFull = static_cast<double>(zInt * 10000000);
		newSector->x = xFull + newSector->radius;
		newSector->y = yFull + newSector->radius;
		newSector->z = zFull + newSector->radius;
		newSector->x_Min = xFull;
		newSector->y_Min = yFull;
		newSector->z_Min = zFull;
		newSector->x_Max = xFull + 2 * newSector->radius;
		newSector->y_Max = yFull + 2 * newSector->radius;
		newSector->z_Max = zFull + 2 * newSector->radius;

		middleIt->second[zInt] = newSector;
		innerIt = middleIt->second.find(zInt);
	}

	return innerIt->second;
}

std::shared_ptr<Sector> SolarSystem::getSector(double x, double y, double z)
{
	const long long int gridSideLength = 10000000;
	long long int xInt = static_cast<long long int>(x / gridSideLength);
	long long int yInt = static_cast<long long int>(y / gridSideLength);
	long long int zInt = static_cast<long long int>(z / gridSideLength);
	// 对取整后的负数坐标进行调整
	if (xInt < 0) xInt -= 1;
	if (yInt < 0) yInt -= 1;
	if (zInt < 0) zInt -= 1;

	// 三层嵌套哈希表的查找逻辑
	std::unordered_map<long long int, std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>>::iterator outerIt;
	std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>::iterator middleIt;
	std::unordered_map<long long int, std::shared_ptr<Sector>>::iterator innerIt;

	// 最外层哈希表查找
	outerIt = m_Sectors.find(xInt);
	if (outerIt != m_Sectors.end()) {
		// 中层哈希表查找
		middleIt = outerIt->second.find(yInt);
		if (middleIt != outerIt->second.end()) {
			// 内层哈希表查找
			innerIt = middleIt->second.find(zInt);
			if (innerIt != middleIt->second.end()) {
				return innerIt->second;
			}
		}
	}
	return nullptr;
}

void SolarSystem::checkObjectsInSector()
{
	// 遍历最外层map的键值对
	for (const auto& outerPair : m_Sectors) {
		const auto& middleMap = outerPair.second;

		// 遍历中层map的键值对
		for (const auto& middlePair : middleMap) {
			const auto& innerMap = middlePair.second;

			// 遍历内层map的键值对
			for (const auto& innerPair : innerMap) {
				const std::shared_ptr<Sector>& sector = innerPair.second;

				size_t size = sector->space_objects.size();
				for (int i = 0; i < size; ++i) {
					const auto object = sector->space_objects[i];
					const auto p = object.lock();
					if (!p)
					{
						sector->space_objects.erase(sector->space_objects.begin() + i);
						// 由于删除了一个元素，索引需要减1，以确保下一次循环能正确检查当前位置的元素

						i--;
						size--;
						continue;
					}

					auto Tran = p->GetComponent<SpaceTransformComponent>();
					auto Base = p->GetComponent<BaseComponent>();

					if (Base->solarSystemID != getSolarSystemID()) {
						for (auto it = space_objects.begin(); it != space_objects.end(); ) {
							if ((*it).lock() == p) {
								space_objects.erase(it);
								(*p_starGateTransferObjects).push_back(p);
								break;
							}
							else {
								++it;
							}
						}

						sector->space_objects.erase(sector->space_objects.begin() + i);
						// 由于删除了一个元素，索引需要减1，以确保下一次循环能正确检查当前位置的元素
						// 从another_space_objects中查找并删除对应的元素，通过比较智能指针

						i--;
						size--;
						continue;
					}

					if (sector->isInSector(Tran->x, Tran->y, Tran->z)) {
						continue;
					}
					else {
						sector->space_objects.erase(sector->space_objects.begin() + i);
						addObjectToSector(p);
						// 由于删除了一个元素，索引需要减1，以确保下一次循环能正确检查当前位置的元素
						i--;
						size--;
					}
				}
			}
		}
	}
}

void SolarSystem::setCurrentSector()
{
	if (currentPilot == nullptr)return;
	auto ship = currentPilot->currentShip;
	auto Tran = ship->GetComponent<SpaceTransformComponent>();
	auto sector = getSector(Tran->x, Tran->y, Tran->z);

	if (sector == nullptr) {
		currentSector = addSector(Tran->x, Tran->y, Tran->z);
		return;
	}
	currentSector = sector;
}

void SolarSystem::clearCurrentSector()
{
	currentSector.reset();
}

std::vector<std::shared_ptr<Pilot>> SolarSystem::getPilots()
{
	return Pilot_objects;
}

void SolarSystem::setCurrentPilots(std::shared_ptr<Pilot> _Pilot)
{
	currentPilot = _Pilot;
	currentPilot->GetComponent<BaseComponent>()->solarSystemID = getSolarSystemID();
	return;
}

void SolarSystem::clearCurrentPilots()
{
	currentPilot.reset();
}