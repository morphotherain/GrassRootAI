#include "AttributesComponent.h"

AttributesComponent::AttributesComponent(int _objectID)
{
	objectID = _objectID;

	typeAttributes = std::make_shared<std::unordered_map<int, Attribute>>();
	objectAttributes = std::make_shared<std::unordered_map<int, Attribute>>();

	auto typeID = dynGameObjectsManager::getInstance()->getTypeIdByObjectID(_objectID);

	typeAttributes = AttributeMgr::getInstance().getTypeAttributesByTypeID(typeID);
	objectAttributes = dynObjectAttributesManager::getInstance()->getAttributesByObjectID(objectID);

	if (objectAttributes->size() < typeAttributes->size()) {
		dynObjectAttributesManager::getInstance()->resetAttributesByObjectID(objectID, *typeAttributes);
		objectAttributes = dynObjectAttributesManager::getInstance()->getAttributesByObjectID(objectID);
	}

}

void AttributesComponent::Init()
{

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
