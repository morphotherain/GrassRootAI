#include "SaveSlotListScreenController.h"
#include "SaveDeleteConfirmScreenController.h"
#include "SaveCreateScreenController.h"
#include "SaveGameManager.h"
#include "UINavigationStack.h"
#include "UIPanelNode.h"
#include "UITextNode.h"
#include "UIButtonNode.h"
#include "UISpriteNode.h"
#include "UISystem.h"
#include "logger_manager.h"
#include <ctime>

namespace
{
	std::string FormatSaveTime(long long unixSeconds)
	{
		if (unixSeconds <= 0)
		{
			return "从未游玩";
		}
		const time_t t = static_cast<time_t>(unixSeconds);
		tm localTime{};
		localtime_s(&localTime, &t);
		char buffer[64] = {};
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &localTime);
		return buffer;
	}
}

SaveSlotListScreenController::SaveSlotListScreenController(UINavigationStack& navigation, UIGameplayHost host)
	: m_navigation(navigation)
	, m_host(std::move(host))
{
}

void SaveSlotListScreenController::OnShow(UISystem& system, UIRenderContext& context)
{
	m_system = &system;
	m_selectedSlotId = -1;

	RegisterAction("save.load", [this]() {
		if (m_selectedSlotId < 0)
		{
			WARN_("未选中存档");
			return;
		}
		if (m_host.onEnterGameFromSlot)
		{
			m_navigation.SetBusy(true);
			m_host.onEnterGameFromSlot(m_selectedSlotId);
		}
	});

	RegisterAction("save.delete", [this]() {
		if (m_selectedSlotId < 0)
		{
			WARN_("未选中存档");
			return;
		}
		m_navigation.Push(
			"resource/ui/save_delete_confirm.ui.json",
			std::make_unique<SaveDeleteConfirmScreenController>(m_navigation, m_host, m_selectedSlotId));
	});

	RegisterAction("nav.open.save_create", [this]() {
		m_navigation.Push(
			"resource/ui/save_create.ui.json",
			std::make_unique<SaveCreateScreenController>(m_navigation, m_host));
	});

	RefreshList(system, context);
	UpdateActionButtons();
}

void SaveSlotListScreenController::RefreshList(UISystem& system, UIRenderContext& context)
{
	auto root = system.GetRoot();
	if (!root)
	{
		return;
	}

	auto containerNode = root->FindById("slot_list_container");
	auto* container = dynamic_cast<UIPanelNode*>(containerNode.get());
	if (!container)
	{
		ERROR_("save_slot_list: 缺少 slot_list_container");
		return;
	}

	container->ClearChildren();
	const auto slots = SaveGameManager::getInstance()->listSaveSlots(false);

	if (auto emptyHint = root->FindById("empty_hint"))
	{
		emptyHint->SetVisible(slots.empty());
	}

	const float rowHeight = 56.0f;
	const float rowGap = 4.0f;
	const float rowWidth = container->GetRect().w;
	float y = 0.0f;

	for (const auto& slot : slots)
	{
		auto row = std::make_shared<UIPanelNode>();
		row->SetId("slot_row_" + std::to_string(slot.slotID));
		row->SetRect({ 0.0f, y, rowWidth, rowHeight });

		auto highlight = std::make_shared<UISpriteNode>();
		highlight->SetId("highlight");
		highlight->SetAtlasId("ui_window");
		highlight->SetFrameName("item_back");
		highlight->SetRect({ 0.0f, 0.0f, rowWidth, rowHeight });
		highlight->SetVisible(slot.slotID == m_selectedSlotId);
		row->AddChild(highlight);

		auto nameText = std::make_shared<UITextNode>();
		nameText->SetId("name");
		nameText->SetTextUtf8(slot.displayName);
		nameText->SetFontSize(22.0f);
		nameText->SetRect({ 12.0f, 6.0f, rowWidth - 24.0f, 26.0f });
		row->AddChild(nameText);

		auto timeText = std::make_shared<UITextNode>();
		timeText->SetId("time");
		timeText->SetTextUtf8(FormatSaveTime(slot.lastPlayedTime));
		timeText->SetFontSize(16.0f);
		timeText->SetRect({ 12.0f, 30.0f, rowWidth - 24.0f, 20.0f });
		row->AddChild(timeText);

		auto selectButton = std::make_shared<UIButtonNode>();
		selectButton->SetId("select");
		selectButton->SetRect({ 0.0f, 0.0f, rowWidth, rowHeight });
		selectButton->SetLabelUtf8("");
		const std::string actionId = "save.select." + std::to_string(slot.slotID);
		selectButton->SetActionId(actionId);
		row->AddChild(selectButton);

		const int slotId = slot.slotID;
		RegisterAction(actionId, [this, slotId]() {
			if (m_system)
			{
				auto* context = m_system->GetRenderContext();
				if (context)
				{
					SelectSlot(slotId, *m_system, *context);
				}
			}
		});

		container->AddChild(row);
		y += rowHeight + rowGap;
	}

	root->Build(context);
}

void SaveSlotListScreenController::SelectSlot(int slotId, UISystem& system, UIRenderContext& context)
{
	m_selectedSlotId = slotId;

	auto root = system.GetRoot();
	if (!root)
	{
		return;
	}

	auto containerNode = root->FindById("slot_list_container");
	if (!containerNode)
	{
		return;
	}

	for (const auto& row : containerNode->GetChildren())
	{
		if (!row)
		{
			continue;
		}
		if (auto highlight = row->FindById("highlight"))
		{
			const bool selected = row->GetId() == ("slot_row_" + std::to_string(slotId));
			highlight->SetVisible(selected);
		}
	}

	UpdateActionButtons();
	(void)context;
}

void SaveSlotListScreenController::UpdateActionButtons()
{
	if (!m_system)
	{
		return;
	}
	auto root = m_system->GetRoot();
	if (!root)
	{
		return;
	}

	const bool hasSelection = m_selectedSlotId >= 0;
	if (auto loadBtn = root->FindById("btn_load"))
	{
		loadBtn->SetVisible(hasSelection);
	}
	if (auto deleteBtn = root->FindById("btn_delete"))
	{
		deleteBtn->SetVisible(hasSelection);
	}
}
