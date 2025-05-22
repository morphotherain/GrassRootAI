

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

#include "AttributeCalculator.h"
#include "AttributeCalculatorManager.h"
#include "AttributeCalculatorMacro.h"


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

    virtual void Update(UINT tick);
    virtual void Init(); 
    virtual void OnDestroy();

    void UpdateAttributes(const std::vector<Attribute>& newAttributes);

    UINT objectID;
    bool shouldUpdate = false;
    void setAttrValueById(int attrID, double value);
    void storeAttributes();

    std::shared_ptr<std::unordered_map<int, Attribute>> typeAttributes;
    std::shared_ptr<std::unordered_map<int, Attribute>> objectAttributes;
    std::shared_ptr < std::unordered_map<int, Attribute>> specificAttributes;
};
