#pragma once

#include <functional>
#include <string>
#include <unordered_map>

class UIActionRegistry
{
public:
	static UIActionRegistry& Instance();

	void Register(const std::string& actionId, std::function<void()> handler);
	void Unregister(const std::string& actionId);
	bool Invoke(const std::string& actionId) const;

private:
	UIActionRegistry() = default;
	std::unordered_map<std::string, std::function<void()>> m_handlers;
};
