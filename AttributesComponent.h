#pragma once

#include "Component.h"
#include "d3dUtil.h"
#include <unordered_map>
#include "AttributeMacro.h"
#include "dogmaAttributesManager.h"
#include "dogmaTypeAttributesManager.h"
#include "dynObjectAttributesManager.h"
#include "dynGameObjectsManager.h"

class AttributeMgr {
public:
	AttributeMgr() = default;
	// 获取单例
	static AttributeMgr& getInstance() {
		static AttributeMgr instance;
		return instance;
	}

	void Init();
	void InitAttributes();

	std::shared_ptr<std::unordered_map<int, Attribute>> getTypeAttributesByTypeID(int typeID);

	std::shared_ptr<std::unordered_map<int, AttributeInfo>> attributes;
	std::shared_ptr< std::unordered_map<int, std::shared_ptr<std::unordered_map<int, Attribute>>>> typeAttributes;
};

class AttributesComponent : public Component {
public:
	AttributesComponent() = default;
	AttributesComponent(int _objectID);
	~AttributesComponent() = default;

	virtual void Update(UINT tick) {};
	virtual void Init();

	UINT objectID;

	std::shared_ptr<std::unordered_map<int, Attribute>> typeAttributes;
	std::shared_ptr<std::unordered_map<int, Attribute>> objectAttributes;
};