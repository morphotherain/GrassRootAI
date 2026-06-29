#pragma once

#include "IUIScreenController.h"
#include "UIGameplayHost.h"
#include "UIScreenTextInput.h"

class UINavigationStack;

class SaveCreateScreenController : public IUIScreenController
{
public:
	SaveCreateScreenController(UINavigationStack& navigation, UIGameplayHost host);

	void OnShow(UISystem& system, UIRenderContext& context) override;
	bool OnChar(wchar_t ch) override;

private:
	UINavigationStack& m_navigation;
	UIGameplayHost m_host;
	UIScreenTextInput m_textInput;
};
