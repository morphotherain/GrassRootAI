#include "SkillComponent.h"
#include "AttributesComponent.h"
#include "BaseComponent.h"
#include "GameObject.h"
#include "TaskMgr.h"

SkillComponent::SkillComponent(UINT _objectID, UINT _ownerId)
    : objectID(_objectID),ownerID(_ownerId) {
    initBaseTaskHandlers();
    initTaskHandlers();

    // 初始化加载技能队列
    activeSkills = dynSkillQueueManager::getInstance()->getActiveSkillsByPilotId(ownerID);
    inactiveSkills = dynSkillQueueManager::getInstance()->getInactiveSkillsByPilotId(ownerID);
}

void SkillComponent::InjectDependency(const std::shared_ptr<Component>& dep) {
    if (auto pComponent = std::dynamic_pointer_cast<AttributesComponent>(dep)) {
        m_pAttributes = pComponent;
    }
    else if (auto pComponent = std::dynamic_pointer_cast<BaseComponent>(dep)) {
        m_pBase = pComponent;
    }
}

void SkillComponent::Update(UINT tick) {
    int skillPointsPerMinute = 40;
    if (!activeSkills.empty()) {
        activeSkills.front().process += skillPointsPerMinute;
    }
}

void SkillComponent::handleTask(const Task& task) {
    taskRegistry.Handle(task);
}

void SkillComponent::OnDestroy() {
    // 组件销毁时可以做一些清理工作
}

void SkillComponent::Refresh() {
    // 刷新技能队列数据
    activeSkills = dynSkillQueueManager::getInstance()->getActiveSkillsByPilotId(ownerID);
    inactiveSkills = dynSkillQueueManager::getInstance()->getInactiveSkillsByPilotId(ownerID);
}

void SkillComponent::initBaseTaskHandlers() {
    taskRegistry.setTypeKey("skillTaskType");

    // 处理添加技能到活跃队列的任务
    taskRegistry.Register("AddToActiveQueue", TASK_HANDLER{
        try {
            int skillTypeId = std::any_cast<int>((*task.paramsPtr)["skillTypeId"]);

            int maxLevel = 0;
            for (auto skill : activeSkills) {
                if (skill.skillTypeId == skillTypeId) {
                    maxLevel = (maxLevel < skill.level) ? skill.level : maxLevel;
                }
            }
            maxLevel++;
            if (maxLevel > 5)return;

            // 检查是否在不活跃队列中
            auto inactiveIt = std::find_if(inactiveSkills.begin(), inactiveSkills.end(),
                [skillTypeId](const DynSkillQueue& data) {
                    return data.skillTypeId == skillTypeId;
                });

            DynSkillQueue newSkill;
            newSkill.skillTypeId = skillTypeId;
            newSkill.pilotId = ownerID;
            newSkill.factor = 1;

            if (inactiveIt != inactiveSkills.end()) {
                // 从非活跃队列移过来，保留进度
                newSkill.process = inactiveIt->process;
                newSkill.level = inactiveIt->level;
                inactiveSkills.erase(inactiveIt);
            }
            else {
                // 新技能，进度为0
                newSkill.process = 0;
                newSkill.level = maxLevel;
            }

            // 计算新优先级（添加到队列末尾）
            newSkill.priority = activeSkills.empty() ? 1 : activeSkills.back().priority + 1;

            // 更新内存中的队列
            activeSkills.push_back(newSkill);

            // 合并活跃和非活跃技能，回写到数据库
            std::vector<DynSkillQueue> allSkills = activeSkills;
            allSkills.insert(allSkills.end(), inactiveSkills.begin(), inactiveSkills.end());
            dynSkillQueueManager::getInstance()->updateSkills(ownerID, allSkills);

            // 发送刷新任务
            auto refreshTask = std::make_shared<Task>();
            refreshTask->publisherId = objectID;
            refreshTask->targetId = objectID;
            (*refreshTask->paramsPtr)["taskType"] = std::string("skillComponent");
            (*refreshTask->paramsPtr)["skillTaskType"] = std::string("Refresh");
            TaskMgr::getInstance().addTask(refreshTask);

        }
    catch (const std::bad_any_cast& e) {
        // 处理参数错误
        return;
    }
    }).Register("RemoveFromActiveQueue", TASK_HANDLER{

        try {
            int skillTypeId = std::any_cast<int>((*task.paramsPtr)["skillTypeId"]);
            int skillLevel = std::any_cast<int>((*task.paramsPtr)["skillLevel"]);

            // 查找技能在活跃队列中的位置
            auto it = std::find_if(activeSkills.begin(), activeSkills.end(),
                [skillTypeId, skillLevel](const DynSkillQueue& data) {
                    return data.skillTypeId == skillTypeId && skillLevel == data.level;
                });

            if (it == activeSkills.end()) {
                // 技能不在活跃队列中
                return;
            }

            DynSkillQueue removedSkill = *it;
            activeSkills.erase(it);

            // 根据进度决定是移除还是移到不活跃队列
            if (removedSkill.process == 0) {
                // 进度为0，直接移除
            }
            else {
                // 有进度，移到不活跃队列
                removedSkill.priority = -1;
                inactiveSkills.push_back(removedSkill);
            }

            // 重新计算活跃队列的优先级（保持连续递增）
            for (size_t i = 0; i < activeSkills.size(); ++i) {
                activeSkills[i].priority = i + 1;
            }

            // 合并活跃和非活跃技能，回写到数据库
            std::vector<DynSkillQueue> allSkills = activeSkills;
            allSkills.insert(allSkills.end(), inactiveSkills.begin(), inactiveSkills.end());
            dynSkillQueueManager::getInstance()->updateSkills(ownerID, allSkills);

            // 发送刷新任务
            auto refreshTask = std::make_shared<Task>();
            refreshTask->publisherId = objectID;
            refreshTask->targetId = objectID;
            (*refreshTask->paramsPtr)["taskType"] = std::string("skillComponent");
            (*refreshTask->paramsPtr)["skillTaskType"] = std::string("Refresh");
            TaskMgr::getInstance().addTask(refreshTask);

                }
        catch (const std::bad_any_cast& e) {
            // 处理参数错误
            return;
        }
    }).Register("Refresh", TASK_HANDLER{
        Refresh();
        });
}
