#include "UIActionScope.h"
#include "UIActionRegistry.h"

void UIActionScope::Register(const std::string& actionId, std::function<void()> handler)
{
	if (actionId.empty())
	{
		return;
	}
	UIActionRegistry::Instance().Register(actionId, std::move(handler));
	m_registeredIds.push_back(actionId);
}

void UIActionScope::Clear()
{
	for (const auto& id : m_registeredIds)
	{
		UIActionRegistry::Instance().Unregister(id);
	}
	m_registeredIds.clear();
}
