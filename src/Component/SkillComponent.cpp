#include "SkillComponent.h"
#include "AttributesComponent.h"
#include "BaseComponent.h"
#include "GameObject.h"
#include "TaskMgr.h"
#include "dynPilotsSkillManager.h"

SkillComponent::SkillComponent(UINT _objectID, UINT _ownerId)
    : objectID(_objectID),ownerID(_ownerId) {
    initBaseTaskHandlers();
    initTaskHandlers();

    // 初始化加载技能队列
    activeSkills = dynSkillQueueManager::getInstance()->getActiveSkillsByPilotId(ownerID);
    inactiveSkills = dynSkillQueueManager::getInstance()->getInactiveSkillsByPilotId(ownerID);
    pilotSkills = dynPilotsSkillManager::getInstance()->getAllSkillsByPilotId(ownerID);
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
    if (!activeSkills.empty() && tick % 60 == 0) {
        // 计算本次更新应增加的技能点（基于每秒点数）
        float pointsToAdd = getSkillPointsPerSecond();
        m_accumulatedPoints += pointsToAdd;

        // 当累积的点数达到1时，转换为整数加到进度中
        if (m_accumulatedPoints >= 1.0f) {
            int integerPoints = static_cast<int>(m_accumulatedPoints);
            activeSkills.front().process += integerPoints;
            m_accumulatedPoints -= integerPoints; // 保留小数部分

            // 检查是否完成当前等级
            checkAndProcessCompletedSkills();
        }
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
            if (!isSkillPrerequisiteMet(skillTypeId))
                return;

            // 1. 查找活跃队列中该技能的最高等级
            int maxActiveLevel = 0;
            for (const auto& skill : activeSkills) {
                if (skill.skillTypeId == skillTypeId) {
                    maxActiveLevel = max(maxActiveLevel, skill.level);
                }
            }

            // 2. 查找已掌握的技能等级（数据库中记录的等级）
            int masteredLevel = 0;
            auto it = std::find_if(pilotSkills.begin(), pilotSkills.end(),
                [skillTypeId](const std::pair<int, int>& skill) {
                    return skill.first == skillTypeId;
                });
            if (it != pilotSkills.end()) {
                masteredLevel = it->second;
            } 
            // 3. 确定新技能的等级：取活跃队列最高等级和已掌握等级中的最大值 + 1
            int newLevel = max(maxActiveLevel, masteredLevel) + 1;

            // 检查是否已达满级
            if (newLevel > 5) {
                return;
            }


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
                if(newLevel == 1)
                    newSkill.process = 0;
                else
                {
                    newSkill.process = getLevelThreshold(newLevel-1);
                }
                newSkill.level = newLevel;
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
            if (removedSkill.process <= getLevelThreshold(removedSkill.level - 1)) {
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

// 在SkillComponent中添加方法：根据累积进度计算当前等级
int SkillComponent::getLevelByProcess(int process) const {
    for (int i = 0; i < LEVEL_THRESHOLDS.size(); ++i) {
        if (process < LEVEL_THRESHOLDS[i]) {
            return i + 1; // 未达到当前阈值，返回当前等级（1-5）
        }
    }
    return 6; // 超过5级阈值（实际不会达到，因最大等级为5）
}

// 获取指定技能的当前等级
int SkillComponent::getCurrentSkillLevel(int skillTypeId) const {
    auto it = std::find_if(pilotSkills.begin(), pilotSkills.end(),
        [skillTypeId](const std::pair<int, int>& skill) {
            return skill.first == skillTypeId;
        });

    if (it != pilotSkills.end()) {
        return it->second;
    }
    return 0; // 默认为0级，表示未学习该技能
}

void SkillComponent::checkAndProcessCompletedSkills() {
    if (activeSkills.empty()) return;

    // 获取队列中优先级最高的技能（队首元素）
    auto& currentSkill = activeSkills.front();
    int skillTypeId = currentSkill.skillTypeId;
    int currentProcess = currentSkill.process;

    // 获取当前已解锁的技能等级
    int currentLevel = getCurrentSkillLevel(skillTypeId);

    // 已达满级，直接移除
    if (currentLevel >= 5) {
        activeSkills.erase(activeSkills.begin());
    }
    else {
        // 检查当前进度是否达到下一级阈值
        int nextLevel = currentLevel + 1;
        int requiredThreshold = getLevelThreshold(nextLevel);

        if (currentProcess >= requiredThreshold) {
            // 升级：更新数据库中的技能等级
            dynPilotsSkillManager::getInstance()->addOrUpdateSkill(
                ownerID, skillTypeId, nextLevel);

            // 更新本地缓存
            auto it = std::find_if(pilotSkills.begin(), pilotSkills.end(),
                [&](const std::pair<int, int>& skill) {
                    return skill.first == skillTypeId;
                });
            if (it != pilotSkills.end()) {
                it->second = nextLevel;
            }
            else {
                pilotSkills.emplace_back(skillTypeId, nextLevel);
            }

            // 从活跃队列中移除已完成的技能等级
            activeSkills.erase(activeSkills.begin());
        }
    }

    // 重新调整剩余技能的优先级
    for (size_t i = 0; i < activeSkills.size(); ++i) {
        activeSkills[i].priority = i + 1; // 优先级从1开始重新编号
    }

    // 保存更新后的队列到数据库
    std::vector<DynSkillQueue> allSkills = activeSkills;
    allSkills.insert(allSkills.end(), inactiveSkills.begin(), inactiveSkills.end());
    dynSkillQueueManager::getInstance()->updateSkills(ownerID, allSkills);
}

// 提取每秒技能点计算方法（方便后期调整计算逻辑）
float SkillComponent::getSkillPointsPerSecond() const {
    // 原逻辑是每分钟40点，转换为每秒点数
    // 后期可根据需求修改（如添加属性加成、技能系数等）
    const int skillPointsPerMinute = 800;
    return static_cast<float>(skillPointsPerMinute) / 60.0f;
}

// 实现方法
std::map<int, int> SkillComponent::getCumulativeCompletionTimes() const {
    std::map<int, int> result;
    if (activeSkills.empty()) {
        return result; // 队列为空，返回空map
    }

    // 获取每秒技能点（用于时间计算）
    float pointsPerSecond = getSkillPointsPerSecond();
    if (pointsPerSecond <= 0) {
        return result; // 避免除以0
    }

    int totalTime = 0; // 累计时间（秒）

    // 遍历活跃技能队列，按优先级顺序计算
    for (const auto& skill : activeSkills) {

        // 计算该技能完成当前等级所需的剩余进度
        int nextLevel = skill.level;
        int threshold = getLevelThreshold(nextLevel) - getLevelThreshold(nextLevel-1);
        int remainingProcess = threshold - skill.process;

        // 计算当前技能所需时间（秒），向上取整
        int skillTime = 0;
        if (remainingProcess > 0) {
            float secondsNeeded = static_cast<float>(remainingProcess) / pointsPerSecond;
            skillTime = static_cast<int>(ceil(secondsNeeded));
        }

        // 累加总时间
        totalTime += skillTime;

        // 记录当前优先级对应的累计时间
        result[skill.priority] = totalTime;
    }

    return result;
}


// 检查指定技能是否满足所有前置要求
bool SkillComponent::isSkillPrerequisiteMet(int targetSkillTypeId) {
    // 1. 获取目标技能的所有前置要求（<前置技能ID, 所需等级>）
    std::map<int, int> prerequisites = InvTypesManager::getInstance()->getReqSkillsById(targetSkillTypeId);
    if (prerequisites.empty()) {
        return true; // 无前置要求，直接满足
    }

    // 2. 检查每个前置技能是否达标
    for (const auto& [prereqSkillId, requiredLevel] : prerequisites) {
        bool isMet = false;

        // 2.1 检查已掌握的技能（pilotSkills）
        auto pilotIt = std::find_if(pilotSkills.begin(), pilotSkills.end(),
            [prereqSkillId](const std::pair<int, int>& skill) {
                return skill.first == prereqSkillId;
            });
        if (pilotIt != pilotSkills.end() && pilotIt->second >= requiredLevel) {
            isMet = true; // 已掌握且等级达标
        }

        // 2.2 检查正在训练的技能（activeSkills，取最高等级）
        if (!isMet) {
            int maxActiveLevel = 0;
            for (const auto& skill : activeSkills) {
                if (skill.skillTypeId == prereqSkillId) {
                    maxActiveLevel = max(maxActiveLevel, skill.level);
                }
            }
            if (maxActiveLevel >= requiredLevel) {
                isMet = true; // 正在训练且等级达标
            }
        }

        // 3. 若有一个前置技能不达标，直接返回false
        if (!isMet) {
            return false;
        }
    }

    // 所有前置技能均达标
    return true;
}