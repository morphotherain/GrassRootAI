#include "UIScreenTextInput.h"
#include "UITextNode.h"
#include <Windows.h>

void UIScreenTextInput::Bind(UITextNode* textNode)
{
	m_textNode = textNode;
}

void UIScreenTextInput::SetTextUtf8(const std::string& text)
{
	m_buffer = text;
	if (m_textNode)
	{
		m_textNode->SetTextUtf8(m_buffer);
	}
}

bool UIScreenTextInput::OnChar(wchar_t ch)
{
	if (ch < 32 && ch != L'\b')
	{
		return false;
	}
	if (ch == L'\b')
	{
		Backspace();
		return true;
	}
	if (ch == L'\r' || ch == L'\n')
	{
		return false;
	}

	char utf8[8] = {};
	const int len = WideCharToMultiByte(CP_UTF8, 0, &ch, 1, utf8, static_cast<int>(sizeof(utf8)), nullptr, nullptr);
	if (len <= 0)
	{
		return false;
	}
	m_buffer.append(utf8, static_cast<size_t>(len));
	if (m_textNode)
	{
		m_textNode->SetTextUtf8(m_buffer);
	}
	return true;
}

void UIScreenTextInput::Backspace()
{
	if (m_buffer.empty())
	{
		return;
	}
	const size_t pos = m_buffer.size();
	size_t start = pos - 1;
	while (start > 0 && (static_cast<unsigned char>(m_buffer[start]) & 0xC0) == 0x80)
	{
		--start;
	}
	m_buffer.erase(start);
	if (m_textNode)
	{
		m_textNode->SetTextUtf8(m_buffer);
	}
}
