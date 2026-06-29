#include "UIButtonNode.h"
#include "UIActionRegistry.h"
#include "UIRenderContext.h"
#include "D3DManager.h"
#include "DXTrace.h"
#include <Windows.h>

void UIButtonNode::SetLabelUtf8(const std::string& label)
{
	if (label.empty())
	{
		m_label.clear();
		return;
	}
	const int size = MultiByteToWideChar(CP_UTF8, 0, label.c_str(), static_cast<int>(label.size()), nullptr, 0);
	if (size <= 0)
	{
		return;
	}
	std::wstring wide(static_cast<size_t>(size), L'\0');
	MultiByteToWideChar(CP_UTF8, 0, label.c_str(), static_cast<int>(label.size()), wide.data(), size);
	m_label = std::move(wide);
}

bool UIButtonNode::Update(UIInputState& input, UIRenderContext& context, float absX, float absY)
{
	if (!m_visible)
	{
		return false;
	}

	const float drawX = absX + m_rect.x;
	const float drawY = absY + m_rect.y;
	const bool inside = input.mouseX >= drawX && input.mouseX <= drawX + m_rect.w
		&& input.mouseY >= drawY && input.mouseY <= drawY + m_rect.h;

	auto self = std::static_pointer_cast<UINode>(shared_from_this());
	auto capture = input.captureNode.lock();

	if (capture && capture.get() != this)
	{
		return false;
	}

	m_hovered = inside;

	if (input.mouseLeftPressed && inside)
	{
		input.captureNode = self;
		m_pressed = true;
		return true;
	}

	if (capture.get() == this)
	{
		if (input.mouseLeftReleased)
		{
			input.captureNode.reset();
			m_pressed = false;
			if (inside && !m_actionId.empty())
			{
				UIActionRegistry::Instance().Invoke(m_actionId);
			}
			return true;
		}
		m_pressed = input.mouseLeftDown;
		return true;
	}

	return UINode::Update(input, context, absX, absY);
}

void UIButtonNode::Draw(UIRenderContext& context, float absX, float absY)
{
	UISpriteNode::Draw(context, absX, absY);

	if (!m_visible || m_label.empty())
	{
		return;
	}

	auto* target = D3DManager::getInstance().GetD2DRenderTarget();
	auto* format = D3DManager::getInstance().GetTextFormat();
	auto* brush = D3DManager::getInstance().GetColorBrush();
	if (!target || !format || !brush)
	{
		return;
	}

	const float drawX = absX + m_rect.x;
	const float drawY = absY + m_rect.y;

	target->BeginDraw();
	target->DrawTextW(
		m_label.c_str(),
		static_cast<UINT32>(m_label.size()),
		format,
		D2D1_RECT_F{ drawX, drawY, drawX + m_rect.w, drawY + m_rect.h },
		brush);
	HR(target->EndDraw());
}
