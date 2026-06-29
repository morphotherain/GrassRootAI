#include "SaveDeleteConfirmScreenController.h"
#include "SaveGameManager.h"
#include "UINavigationStack.h"
#include "UITextNode.h"
#include "UISystem.h"

SaveDeleteConfirmScreenController::SaveDeleteConfirmScreenController(
	UINavigationStack& navigation, UIGameplayHost host, int slotId)
	: m_navigation(navigation)
	, m_host(std::move(host))
	, m_slotId(slotId)
{
}

void SaveDeleteConfirmScreenController::OnShow(UISystem& system, UIRenderContext& context)
{
	(void)context;

	SaveSlotInfo info{};
	if (SaveGameManager::getInstance()->getSaveSlotInfo(m_slotId, info))
	{
		if (auto root = system.GetRoot())
		{
			if (auto message = root->FindById("confirm_message"))
			{
				if (auto* text = dynamic_cast<UITextNode*>(message.get()))
				{
					text->SetTextUtf8("确定删除存档「" + info.displayName + "」？");
				}
			}
		}
	}

	RegisterAction("save.delete.confirm", [this]() {
		if (m_host.onDeleteSaveSlot && m_host.onDeleteSaveSlot(m_slotId))
		{
			m_navigation.Pop();
		}
	});
}
