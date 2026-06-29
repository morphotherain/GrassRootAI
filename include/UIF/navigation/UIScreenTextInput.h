#pragma once

#include <string>

class UITextNode;

// 轻量文本输入：绑定 UITextNode，供弹窗/表单复用（不新增节点类型）。
class UIScreenTextInput
{
public:
	void Bind(UITextNode* textNode);
	void SetTextUtf8(const std::string& text);
	const std::string& GetTextUtf8() const { return m_buffer; }
	bool OnChar(wchar_t ch);
	void Backspace();

private:
	UITextNode* m_textNode = nullptr;
	std::string m_buffer;
};
