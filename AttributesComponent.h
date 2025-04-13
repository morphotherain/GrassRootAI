//#pragma once
//
//#include "Component.h"
//#include "d3dUtil.h"
//#include <unordered_map>
//#include "AttributeMacro.h"
//#include "dogmaAttributesManager.h"
//#include "dogmaTypeAttributesManager.h"
//#include "dynObjectAttributesManager.h"
//#include "dynGameObjectsManager.h"
//
//class AttributeMgr {
//public:
//	AttributeMgr() = default;
//	// 获取单例
//	static AttributeMgr& getInstance() {
//		static AttributeMgr instance;
//		return instance;
//	}
//
//	void Init();
//	void InitAttributes();
//
//	std::shared_ptr<std::unordered_map<int, Attribute>> getTypeAttributesByTypeID(int typeID);
//
//	std::shared_ptr<std::unordered_map<int, AttributeInfo>> attributes;
//	std::shared_ptr< std::unordered_map<int, std::shared_ptr<std::unordered_map<int, Attribute>>>> typeAttributes;
//};
//
//class AttributesComponent : public Component {
//public:
//	AttributesComponent() = default;
//	AttributesComponent(int _objectID);
//	~AttributesComponent() = default;
//
//	std::vector<std::type_index> GetDependencies() const override {
//		return {};
//	}
//	void InjectDependency(const std::shared_ptr<Component>& dep) override {}
//
//	virtual void Update(UINT tick) {};
//	virtual void Init();
//
//	UINT objectID;
//
//	std::shared_ptr<std::unordered_map<int, Attribute>> typeAttributes;
//	std::shared_ptr<std::unordered_map<int, Attribute>> objectAttributes;
//};

#pragma once

#include "Component.h"
#include "d3dUtil.h"
#include <unordered_map>
#include "AttributeMacro.h"
#include "dogmaAttributesManager.h"
#include "dogmaTypeAttributesManager.h"
#include "dynObjectAttributesManager.h"
#include "dynGameObjectsManager.h"
#include <unordered_set>

// 定义需要存储在动态表中的特定属性
const std::unordered_set<int> SPECIFIC_ATTRIBUTES = {
    ATTR_ID_QUANTITY, ATTR_ID_CURRENT_HP, ATTR_ID_CURRENT_ARMOR, ATTR_ID_CURRENT_SHIELD
};

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

    std::vector<std::type_index> GetDependencies() const override {
        return {};
    }
    void InjectDependency(const std::shared_ptr<Component>& dep) override {}

    virtual void Update(UINT tick) {};
    virtual void Init(); 
    
    void UpdateAttributes(const std::vector<Attribute>& newAttributes);

    UINT objectID;

    std::shared_ptr<std::unordered_map<int, Attribute>> typeAttributes;
    std::shared_ptr<std::unordered_map<int, Attribute>> objectAttributes;
};
