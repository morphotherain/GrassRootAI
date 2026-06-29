#include "UITextNode.h"
#include "UIRenderContext.h"
#include "D3DManager.h"
#include "DXTrace.h"
#include <Windows.h>
#include <d2d1helper.h>

void UITextNode::SetTextUtf8(const std::string& text)
{
	if (text.empty())
	{
		m_text.clear();
		return;
	}
	const int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0);
	if (size <= 0)
	{
		return;
	}
	std::wstring wide(static_cast<size_t>(size), L'\0');
	MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), wide.data(), size);
	m_text = std::move(wide);
}

void UITextNode::SetFontSize(float fontSize)
{
	m_fontSize = (fontSize > 1.0f) ? fontSize : 1.0f;
	RebuildTextFormat();
}

void UITextNode::RebuildTextFormat()
{
	m_textFormat.Reset();
	auto* factory = D3DManager::getInstance().GetDWriteFactory();
	if (!factory)
	{
		return;
	}
	HR(factory->CreateTextFormat(
		L"黑体",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		m_fontSize,
		L"zh-cn",
		m_textFormat.GetAddressOf()));
	if (m_textFormat)
	{
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
}

void UITextNode::Build(UIRenderContext& context)
{
	(void)context;
	RebuildTextFormat();
	UINode::Build(context);
}

void UITextNode::Draw(UIRenderContext& context, float absX, float absY)
{
	if (!m_visible || m_text.empty())
	{
		return;
	}

	auto* target = D3DManager::getInstance().GetD2DRenderTarget();
	auto* format = m_textFormat.Get();
	if (!format)
	{
		format = D3DManager::getInstance().GetTextFormat();
	}
	auto* brush = D3DManager::getInstance().GetColorBrush();
	if (!target || !format || !brush)
	{
		return;
	}

	const float drawX = absX + m_rect.x;
	const float drawY = absY + m_rect.y;
	const float pivotX = drawX + m_rect.w * 0.5f;
	const float pivotY = drawY + m_rect.h * 0.5f;

	D2D1_MATRIX_3X2_F oldTransform{};
	target->GetTransform(&oldTransform);

	D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Translation(-pivotX, -pivotY)
		* D2D1::Matrix3x2F::Rotation(GetRotation())
		* D2D1::Matrix3x2F::Scale(GetScaleX(), GetScaleY())
		* D2D1::Matrix3x2F::Translation(pivotX, pivotY);
	target->SetTransform(transform * oldTransform);

	target->BeginDraw();
	target->DrawTextW(
		m_text.c_str(),
		static_cast<UINT32>(m_text.size()),
		format,
		D2D1_RECT_F{ drawX, drawY, drawX + m_rect.w, drawY + m_rect.h },
		brush);
	HR(target->EndDraw());

	target->SetTransform(oldTransform);
}
