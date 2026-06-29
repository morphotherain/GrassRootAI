#pragma once

#include <functional>
#include <string>
#include <vector>

// 屏幕级 action 注册：OnHide 时批量 Unregister，避免泄漏与 id 冲突。
class UIActionScope
{
public:
	void Register(const std::string& actionId, std::function<void()> handler);
	void Clear();

private:
	std::vector<std::string> m_registeredIds;
};
