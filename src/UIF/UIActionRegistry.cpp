#include "UIActionRegistry.h"

UIActionRegistry& UIActionRegistry::Instance()
{
	static UIActionRegistry instance;
	return instance;
}

void UIActionRegistry::Register(const std::string& actionId, std::function<void()> handler)
{
	m_handlers[actionId] = std::move(handler);
}

void UIActionRegistry::Unregister(const std::string& actionId)
{
	m_handlers.erase(actionId);
}

bool UIActionRegistry::Invoke(const std::string& actionId) const
{
	auto it = m_handlers.find(actionId);
	if (it == m_handlers.end() || !it->second)
	{
		return false;
	}
	it->second();
	return true;
}
