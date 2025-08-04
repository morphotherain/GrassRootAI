#pragma once

#include "Component.h"
#include "StorageMacro.h"
#include "AttributesComponent.h"
#include "BaseComponent.h"
#include "d3dUtil.h"
#include "TaskHandlerRegistry.h"
#include <vector>
#include <memory>
#include "dynSkillQueueManager.h"

// 前向声明
class AttributesComponent;
class BaseComponent;

class SkillComponent : public Component {
public:
    SkillComponent() = default;
    SkillComponent(UINT _objectID, UINT _ownerId);
    ~SkillComponent() = default;

    std::vector<std::type_index> GetDependencies() const override {
        return {
            typeid(AttributesComponent),
            typeid(BaseComponent)
        };
    }

    void InjectDependency(const std::shared_ptr<Component>& dep) override;
    void Update(UINT tick) override;
    void handleTask(const Task& task) override;
    void OnDestroy() override;
    void Refresh();

    std::vector<DynSkillQueue>* getSkillQueue() { return &activeSkills; }

    UINT objectID;
    UINT ownerID;;
    std::vector<DynSkillQueue> activeSkills;      // 活跃技能队列（优先级不为-1）
    std::vector<DynSkillQueue> inactiveSkills;    // 不活跃技能（优先级为-1）

private:
    void initBaseTaskHandlers();
    virtual void initTaskHandlers() {}

    std::shared_ptr<AttributesComponent> m_pAttributes;
    std::shared_ptr<BaseComponent> m_pBase;
    TaskHandlerRegistry taskRegistry;
};
