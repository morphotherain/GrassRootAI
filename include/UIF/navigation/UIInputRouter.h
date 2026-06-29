#pragma once

#include <functional>

// WM_CHAR 路由：编辑器未消费时交给当前 Screen。
class UIInputRouter
{
public:
	static UIInputRouter& Instance();

	void SetCharHandler(std::function<bool(wchar_t)> handler);
	bool OnChar(wchar_t ch);

private:
	UIInputRouter() = default;
	std::function<bool(wchar_t)> m_charHandler;
};
