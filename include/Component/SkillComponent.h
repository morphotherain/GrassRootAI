#pragma once

#include "Component.h"
#include "StorageMacro.h"
#include "AttributesComponent.h"
#include "BaseComponent.h"
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

    void checkAndProcessCompletedSkills();
    int getCurrentSkillLevel(int skillTypeId) const;

    // 判断指定技能是否满足前置要求
    bool isSkillPrerequisiteMet(int targetSkillTypeId);

    // 各级所需的总进度阈值（一级需要250，二级需要1414，以此类推）
    const std::vector<int> LEVEL_THRESHOLDS = { 250, 1414, 8000, 45255, 256000 };
    // 获取指定等级所需的总进度（level为1-5级）
    int getLevelThreshold(int level) const {
        if (level < 1 || level > 5) return 0;
        return LEVEL_THRESHOLDS[level - 1]; // 索引0对应1级，索引4对应5级
    }
    int getLevelByProcess(int process) const;
    // 计算完成需要的时间（秒）
    std::map<int, int> getCumulativeCompletionTimes() const;
    // 计算每秒获得的技能点数量（提取为独立方法，方便后期修改）
    float getSkillPointsPerSecond() const;

    UINT objectID;
    UINT ownerID;;
    std::vector<DynSkillQueue> activeSkills;      // 活跃技能队列（优先级不为-1）
    std::vector<DynSkillQueue> inactiveSkills;    // 不活跃技能（优先级为-1）
    std::vector<std::pair<int, int>> pilotSkills;  // 已拥有技能 

private:
    void initBaseTaskHandlers();
    virtual void initTaskHandlers() {}


    std::shared_ptr<AttributesComponent> m_pAttributes;
    std::shared_ptr<BaseComponent> m_pBase;
    TaskHandlerRegistry taskRegistry;
private:
    float m_accumulatedPoints = 0.0f; // 累积的技能点小数部分
};
