#include "SolarSystem.h"
#include "InvTypesManager.h"
#include "invGroupsManager.h"
#include "eveBracketsManager.h"


void SolarSystem::Init()
{
    getDenormalizesBySolarSystemID();
    auto p_vGameObjectData = dynGameObjectsManager::getInstance()->getGameObjectBySolarSystemID(m_solarSystem.solarSystemID);
    for (auto& objectData : *p_vGameObjectData) {
        addGameObject(objectData);
        
    }
    for (auto p_object : space_objects) {
        addObjectToSector(p_object);
    }
    for (auto p_object : other_objects) {
    }

}

void SolarSystem::Update(UINT tick)
{
    

    for (auto obj : space_objects) {
        obj->Update(tick);
    }

    if (tick % 60 == 0) {
        checkObjectsInSector();
        setCurrentSector();
    }



    //Tran->x -= 16;

}



void SolarSystem::getDenormalizesBySolarSystemID()
{

    // 获取数据库实例
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager->getDatabase();

    // SQL 查询语句，获取所有恒星系的 x, y, z, solarSystemName 和 luminosity
    std::string sql = "SELECT x, y, z, nameID, regionID ,constellationID ,solarSystemID,radius,itemID,mapDenormalize.typeID,celestialIndex,orbitIndex, invtypes.groupID, invGroups.categoryID FROM mapDenormalize JOIN invtypes ON mapDenormalize.typeID = invtypes.typeID JOIN invGroups ON invtypes.groupID = invGroups.groupID WHERE solarSystemID = " + std::to_string(m_solarSystem.solarSystemID) + ";";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        auto temp = sqlite3_errmsg(db);
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }

    // 迭代查询结果并将数据存储到 solarSystems 结构中
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        auto p_denormalize = std::make_shared<DenormalizeData>();
        dynGameObject objectData;

        // 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
        p_denormalize->x = sqlite3_column_double(stmt, 0);
        p_denormalize->y = sqlite3_column_double(stmt, 1);
        p_denormalize->z = sqlite3_column_double(stmt, 2);
        p_denormalize->nameID = sqlite3_column_int(stmt, 3); // 获取 wstring 类型的名称
        p_denormalize->regionID = sqlite3_column_int(stmt, 4);
        p_denormalize->constellationID = sqlite3_column_int(stmt, 5);
        p_denormalize->solarSystemID = sqlite3_column_int(stmt, 6);
        p_denormalize->radius = sqlite3_column_double(stmt, 7);
        p_denormalize->itemID = sqlite3_column_int(stmt, 8);
        p_denormalize->typeID = sqlite3_column_int(stmt, 9);
        p_denormalize->celestialIndex = sqlite3_column_int(stmt, 10);
        p_denormalize->orbitIndex = sqlite3_column_int(stmt, 11);

        m_denormalizes.push_back(p_denormalize);

        objectData.x = p_denormalize->x;
        objectData.y = p_denormalize->y;
        objectData.z = p_denormalize->z;
        objectData.SolarSystemID = p_denormalize->solarSystemID;
        objectData.ContainerID = p_denormalize->solarSystemID;
        objectData.objectID = p_denormalize->itemID;
        objectData.typeID = p_denormalize->typeID;
        objectData.groupID = sqlite3_column_int(stmt, 12);
        objectData.categoryID = sqlite3_column_int(stmt, 13);
        addGameObject(objectData);
    }

    // 释放语句资源
    sqlite3_finalize(stmt);

    for (auto p_denormalize : m_denormalizes) {
        p_denormalize->name = dbManager->getNameByTypeId(p_denormalize->typeID);
        p_denormalize->groupID = InvTypesManager::getInstance()->getGroupByTypeId(p_denormalize->typeID);
        p_denormalize->bracketID = invGroupsManager::getInstance()->getBracketIDByGroupId(p_denormalize->groupID);
        p_denormalize->dds_path = eveBracketsManager::getInstance()->getPathByTypeId(p_denormalize->bracketID);
    }

}

void SolarSystem::addGameObject(dynGameObject& objectData)
{
    std::shared_ptr<GameObject> object;
    switch (objectData.categoryID) {
    case 2: {    //天体
        object = std::make_shared<Astro>(objectData.objectID);
        break;
    }
    case 3: {    //NPC 空间站
        object = std::make_shared<NPCStation>(objectData.objectID);
        break;
    }
    case 5: {    //附件(克隆人飞行员)
        object = std::make_shared<Pilot>(objectData.objectID, objectData.OwnerID);
        other_objects.push_back(object);
        Pilot_objects.push_back(std::dynamic_pointer_cast<Pilot>(object));
        break;
    }
    case 6: {    //舰船(含太空舱
        object = std::make_shared<Ship>(objectData.objectID);
        break;
    }
    default: {
        return;
    }
    }
    auto p_object = object->ConvertBasedOnGroupID(objectData.groupID);
    if (p_object != nullptr)object = p_object;
    object->Init();
    map_objects[objectData.objectID] = object;
    (*p_mapObject)[objectData.objectID] = object;
    object->objectID = objectData.objectID;
    if (object->GetComponent<SpaceTransformComponent>() != nullptr) {
        space_objects.push_back(object);
    }
    if (object->GetComponent<BaseComponent>() != nullptr) {
        auto base = object->GetComponent<BaseComponent>();
        base->objectID = objectData.objectID;
        base->typeID = objectData.typeID;
        base->ownerID = objectData.OwnerID;
        base->containerID = objectData.ContainerID;
        base->solarSystemID = objectData.SolarSystemID;
        base->groupID = objectData.groupID;
        base->categoryID = objectData.categoryID;
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
        long long int xFull = xInt * 10000000;
        long long int yFull = yInt * 10000000;
        long long int zFull = zInt * 10000000;
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

                int size = sector->space_objects.size();
                for (int i = 0; i < size; ++i) {
                    const auto& object = sector->space_objects[i];

                    auto Tran = object->GetComponent<SpaceTransformComponent>();

                    if (sector->isInSector(Tran->x, Tran->y, Tran->z)) {
                        continue;
                    }
                    else {
                        sector->space_objects.erase(sector->space_objects.begin() + i);
                        addObjectToSector(object);
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
    currentSector = getSector(Tran->x, Tran->y, Tran->z);
}

std::vector<std::shared_ptr<Pilot>> SolarSystem::getPilots()
{

    return Pilot_objects;
}

void SolarSystem::setCurrentPilots(std::shared_ptr<Pilot> _Pilot)
{
    currentPilot = _Pilot;
    UINT shipID = currentPilot->GetComponent<BaseComponent>()->containerID;
    if (map_objects.find(shipID) != map_objects.end())
    {
        currentPilot->currentShip = std::reinterpret_pointer_cast<Ship>(map_objects[shipID]);
    }
    return;
}
