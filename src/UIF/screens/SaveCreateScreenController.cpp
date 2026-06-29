#include "SaveCreateScreenController.h"
#include "UINavigationStack.h"
#include "UITextNode.h"
#include "UISystem.h"

SaveCreateScreenController::SaveCreateScreenController(UINavigationStack& navigation, UIGameplayHost host)
	: m_navigation(navigation)
	, m_host(std::move(host))
{
}

void SaveCreateScreenController::OnShow(UISystem& system, UIRenderContext& context)
{
	(void)context;

	auto root = system.GetRoot();
	if (!root)
	{
		return;
	}

	auto* inputNode = dynamic_cast<UITextNode*>(root->FindById("input_display_name").get());
	m_textInput.Bind(inputNode);
	m_textInput.SetTextUtf8("新建存档");

	RegisterAction("save.create.confirm", [this]() {
		const std::string& name = m_textInput.GetTextUtf8();
		if (name.empty())
		{
			return;
		}
		if (m_host.onCreateAndEnterGame)
		{
			m_navigation.SetBusy(true);
			m_host.onCreateAndEnterGame(name);
		}
	});
}

bool SaveCreateScreenController::OnChar(wchar_t ch)
{
	return m_textInput.OnChar(ch);
}
