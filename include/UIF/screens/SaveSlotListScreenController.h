#pragma once

#include "IUIScreenController.h"
#include "UIGameplayHost.h"

class UINavigationStack;

class SaveSlotListScreenController : public IUIScreenController
{
public:
	SaveSlotListScreenController(UINavigationStack& navigation, UIGameplayHost host);

	void OnShow(UISystem& system, UIRenderContext& context) override;

private:
	void RefreshList(UISystem& system, UIRenderContext& context);
	void SelectSlot(int slotId, UISystem& system, UIRenderContext& context);
	void UpdateActionButtons();

	UINavigationStack& m_navigation;
	UIGameplayHost m_host;
	int m_selectedSlotId = -1;
	UISystem* m_system = nullptr;
};
