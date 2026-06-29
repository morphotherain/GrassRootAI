#include "UIInputRouter.h"

UIInputRouter& UIInputRouter::Instance()
{
	static UIInputRouter instance;
	return instance;
}

void UIInputRouter::SetCharHandler(std::function<bool(wchar_t)> handler)
{
	m_charHandler = std::move(handler);
}

bool UIInputRouter::OnChar(wchar_t ch)
{
	if (m_charHandler)
	{
		return m_charHandler(ch);
	}
	return false;
}
