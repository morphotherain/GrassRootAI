#pragma once

#include "IUIScreenController.h"
#include "UIGameplayHost.h"

class UINavigationStack;

class SaveDeleteConfirmScreenController : public IUIScreenController
{
public:
	SaveDeleteConfirmScreenController(UINavigationStack& navigation, UIGameplayHost host, int slotId);

	void OnShow(UISystem& system, UIRenderContext& context) override;

private:
	UINavigationStack& m_navigation;
	UIGameplayHost m_host;
	int m_slotId = -1;
};
