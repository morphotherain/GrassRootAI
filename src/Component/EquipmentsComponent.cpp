#include "EquipmentsComponent.h"
#include "invEquipmentGroupSlotsManager.h"
#include "Task/TaskParams.h"
#include "Sim/IAssetLocationService.h"

EquipmentsComponent::EquipmentsComponent(UINT _objectID)
{
	objectID = _objectID;
}

void EquipmentsComponent::InjectDependency(const std::shared_ptr<Component>& dep)
{
	if (auto pComponent = std::dynamic_pointer_cast<AttributesComponent>(dep)) {
		m_pAttributes = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<LockingComponent>(dep)) {
		m_pLocking = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<HighSlotComponent>(dep)) {
		m_pHighSlot = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<MediumSlotComponent>(dep)) {
		m_pMediumSlot = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<LowSlotComponent>(dep)) {
		m_pLowSlot = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<RigSlotComponent>(dep)) {
		m_pRigSlot = pComponent;
	}
}

void EquipmentsComponent::Update(UINT tick)
{
}

void EquipmentsComponent::handleTask(const Task& task)
{
	try {
		const auto taskType = ReadEntityTaskType(task);
		if (auto switchParams = TryReadEquipSwitchParams(task)) {
			const auto& slotType = switchParams->slotType;
			const auto slotIndex = switchParams->slotIndex;

			DEBUG_("slotType, taskType, slotIndex : {}{}{}", slotType, taskType, slotIndex);
			int targetObjectID = -1;
			if (slotType == "high") {
				if (slotIndex >= 0 && slotIndex < static_cast<int>(m_pHighSlot->itemIDs.size())) {
					targetObjectID = m_pHighSlot->itemIDs[slotIndex];
				}
			}
			if (slotType == "medium") {
				if (slotIndex >= 0 && slotIndex < static_cast<int>(m_pMediumSlot->itemIDs.size())) {
					targetObjectID = m_pMediumSlot->itemIDs[slotIndex];
				}
			}
			if (slotType == "low") {
				if (slotIndex >= 0 && slotIndex < static_cast<int>(m_pLowSlot->itemIDs.size())) {
					targetObjectID = m_pLowSlot->itemIDs[slotIndex];
				}
			}
			auto target = GameObjectMgr::getInstance().getObject(targetObjectID);
			TaskMgr::getInstance().addTask(
				TaskFactory::MakeTargetObjectIdTask(objectID, target, m_pLocking->currentLockedTargetId));
		}

		if (task.getParamOrDefault<std::string>("equipmentTaskType", "") == "installEquipment") {
			const auto installParams = ReadEquipInstallParams(task);
			const auto equipObjectID = installParams.objectID;
			const auto groupID = installParams.groupID;
			int slot = invEquipmentGroupSlotsManager::getInstance()->getSlotByGroupID(groupID);
            if (slot == 0) return;
            int bagId = 0;
			int numEquip = 0;
			int maxNum = 0;
            switch (slot)
            {
            case 0:
                return;
                break;
            case 1: {
                bagId = m_pHighSlot->bagId;
				numEquip = m_pHighSlot->itemIDs.size();
				maxNum = m_pAttributes->getAttrValueById(ATTR_ID_HI_SLOTS, 0);
                break;
            }
            case 2: {
                bagId = m_pMediumSlot->bagId;
				numEquip = m_pMediumSlot->itemIDs.size();
				maxNum = m_pAttributes->getAttrValueById(ATTR_ID_MED_SLOTS, 0);
                break;
            }
            case 3: {
                bagId = m_pLowSlot->bagId;
				numEquip = m_pLowSlot->itemIDs.size();
				maxNum = m_pAttributes->getAttrValueById(ATTR_ID_LOW_SLOTS, 0);
                break;
            }
            case 4: {
                bagId = m_pRigSlot->bagId;
				numEquip = m_pRigSlot->itemIDs.size();
				maxNum = m_pAttributes->getAttrValueById(ATTR_ID_RIG_SLOTS, 0);
                break;
            }
                
            default:
                return;
                break;
            }
			if (bagId != 0) {
				if(maxNum > numEquip)
					GetAssetLocationService().MoveToBag(
						static_cast<std::uint32_t>(equipObjectID),
						static_cast<std::uint32_t>(bagId));
			}
			Refresh();
		}
		if (taskType == "uninstallEquipment") {

		}
	}
	catch (const std::bad_any_cast& e) {
		// 记录日志或进行错误处理
		DEBUG_("类型转换错误: {}", e.what());
	}


}


void EquipmentsComponent::Refresh() {
	m_pHighSlot->Refresh();
	m_pMediumSlot->Refresh();
	m_pLowSlot->Refresh();
	m_pRigSlot->Refresh();
}