

#include "AttributesComponent.h"

AttributesComponent::AttributesComponent(int _objectID)
{
    objectID = _objectID;

    typeAttributes = std::make_shared<std::unordered_map<int, Attribute>>();
    objectAttributes = std::make_shared<std::unordered_map<int, Attribute>>();

    auto typeID = dynGameObjectsManager::getInstance()->getTypeIdByObjectID(_objectID);

    typeAttributes = AttributeMgr::getInstance().getTypeAttributesByTypeID(typeID);
    objectAttributes = dynObjectAttributesManager::getInstance()->getAttributesByObjectID(objectID);
    specificAttributes = std::make_shared< std::unordered_map<int, Attribute>>();

    // 只将特定属性存入动态表
    for (const auto& attrPair : *typeAttributes) {
        if (SPECIFIC_ATTRIBUTES.count(attrPair.first) > 0) {
            (*specificAttributes)[attrPair.first] = attrPair.second;
        }
    }

    // 额外添加特定属性逻辑
    std::unordered_map<int, std::pair<int, double>> additionalAttrMapping = {
        {ATTR_ID_STRUCTURE_VALUE, {ATTR_ID_CURRENT_HP, 100.0}},  // 结构值对应当前HP，初始值设为100.0
        {ATTR_ID_ARMOR_HP, {ATTR_ID_CURRENT_ARMOR, 50.0}},  // 装甲值对应当前装甲，初始值设为50.0
        {ATTR_ID_SHIELD_CAPACITY, {ATTR_ID_CURRENT_SHIELD, 80.0}},  // 护盾容量对应当前护盾，初始值设为80.0
        {ATTR_ID_VOLUME, {ATTR_ID_QUANTITY, 1.0}}  // 体积对应数量，初始值设为1.0
    };
    for (const auto& mapping : additionalAttrMapping) {
        if (typeAttributes->find(mapping.first) != typeAttributes->end()) {
            Attribute newAttr;
            newAttr.attributeID = mapping.second.first;
            if (mapping.first == ATTR_ID_STRUCTURE_VALUE || mapping.first == ATTR_ID_CURRENT_ARMOR || mapping.first == ATTR_ID_SHIELD_CAPACITY)
                newAttr.value = (*typeAttributes)[mapping.first].value;
            else
                newAttr.value = mapping.second.second;
            (*specificAttributes)[mapping.second.first] = newAttr;
        }
    }

    if (objectAttributes->size() < (*specificAttributes).size()) {
        dynObjectAttributesManager::getInstance()->resetAttributesByObjectID(objectID, (*specificAttributes));
        objectAttributes = dynObjectAttributesManager::getInstance()->getAttributesByObjectID(objectID);
    }

    // 合并类型属性和对象属性
    for (const auto& attrPair : *typeAttributes) {
        if (objectAttributes->find(attrPair.first) == objectAttributes->end()) {
            (*objectAttributes)[attrPair.first] = attrPair.second;
        }
    }
}

void AttributesComponent::Update(UINT tick)
{
    if (tick % 30 == 0) {
        if (shouldUpdate) {
            storeAttributes();
            shouldUpdate = false;
        }
    }
}

void AttributesComponent::Init()
{
}

void AttributesComponent::UpdateAttributes(const std::vector<Attribute>& newAttributes)
{
    for (const auto& attr : newAttributes) {
        (*objectAttributes)[attr.attributeID] = attr;
    }
    shouldUpdate = true;
}

void AttributesComponent::storeAttributes()
{
    for (auto& attrPair : *specificAttributes) {
        if (objectAttributes->find(attrPair.first) != objectAttributes->end()) {
            attrPair.second = (*objectAttributes)[attrPair.first];
        }
    }
    dynObjectAttributesManager::getInstance()->updateAttributesByObjectID(objectID, (*specificAttributes));
    shouldUpdate = false;
}

void AttributesComponent::setAttrValueById(int attrID, double value)
{
    if(objectAttributes->find(attrID)!= objectAttributes->end())
    {
        (*objectAttributes)[attrID].value = value;
        shouldUpdate = true;
    }
}

void AttributesComponent::OnDestroy()
{
    dynObjectAttributesManager::getInstance()->removeAttributesByObjectID(objectID);
}

void AttributeMgr::Init()
{
    InitAttributes();
}

void AttributeMgr::InitAttributes()
{
    attributes = dogmaAttributesManager::getInstance()->getAttributes();
    typeAttributes = std::make_shared< std::unordered_map<int, std::shared_ptr<std::unordered_map<int, Attribute>>>>();
}


std::shared_ptr<std::unordered_map<int, Attribute>> AttributeMgr::getTypeAttributesByTypeID(int typeID)
{
    auto it = typeAttributes->find(typeID);
    if (it == typeAttributes->end()) {
        (*typeAttributes)[typeID] = dogmaTypeAttributesManager::getInstance()->getTypeAttributesByTypeId(typeID);
        return (*typeAttributes)[typeID];
    }
    else {
        return (*typeAttributes)[typeID];
    }
}
