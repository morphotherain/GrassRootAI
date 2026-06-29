#pragma once

#include "IUIScreenController.h"
#include "UIGameplayHost.h"

class UINavigationStack;

class MainMenuScreenController : public IUIScreenController
{
public:
	MainMenuScreenController(UINavigationStack& navigation, UIGameplayHost host);

	void OnShow(UISystem& system, UIRenderContext& context) override;

private:
	UINavigationStack& m_navigation;
	UIGameplayHost m_host;
};
