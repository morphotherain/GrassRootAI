#include "UIEditorRenderer.h"
#include "UIEditorInspector.h"
#include "UIEditorLayout.h"
#include "UIEditorTextUtil.h"
#include "UIButtonNode.h"
#include "UIPanelNode.h"
#include "UISpriteNode.h"
#include "UITextNode.h"
#include <sstream>
#include <string>

static void DrawFilledRect(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush,
	float x, float y, float w, float h, D2D1_COLOR_F color)
{
	if (!target || !brush)
	{
		return;
	}
	brush->SetColor(color);
	target->FillRectangle(D2D1_RECT_F{ x, y, x + w, y + h }, brush);
}

static void DrawRectOutline(ID2D1RenderTarget* target, ID2D1SolidColorBrush* brush,
	float x, float y, float w, float h, D2D1_COLOR_F color, float strokeWidth)
{
	if (!target || !brush)
	{
		return;
	}
	brush->SetColor(color);
	D2D1_RECT_F rect = { x, y, x + w, y + h };
	target->DrawRectangle(rect, brush, strokeWidth);
}

static void DrawTextLine(UIRenderContext& context, const std::wstring& text, float x, float y, float maxWidth = 900.0f)
{
	auto* target = context.GetD2DTarget();
	auto* format = context.GetTextFormat();
	auto* brush = context.GetBrush();
	if (!target || !format || !brush)
	{
		return;
	}
	target->DrawTextW(
		text.c_str(),
		static_cast<UINT32>(text.size()),
		format,
		D2D1_RECT_F{ x, y, x + maxWidth, y + 22.0f },
		brush);
}

std::string UIEditorRenderer::GetNodeTypeName(const UINode& node)
{
	if (dynamic_cast<const UIButtonNode*>(&node))
	{
		return "Button";
	}
	if (dynamic_cast<const UITextNode*>(&node))
	{
		return "Text";
	}
	if (dynamic_cast<const UISpriteNode*>(&node))
	{
		return "Sprite";
	}
	if (dynamic_cast<const UIPanelNode*>(&node))
	{
		return "Panel";
	}
	return "Node";
}

void UIEditorRenderer::DrawDimOverlay(UIRenderContext& context)
{
	DrawFilledRect(
		context.GetD2DTarget(),
		context.GetBrush(),
		0.0f,
		0.0f,
		static_cast<float>(context.GetClientWidth()),
		static_cast<float>(context.GetClientHeight()),
		D2D1_COLOR_F{ 0.0f, 0.0f, 0.0f, 0.25f });
}

void UIEditorRenderer::DrawToolbar(UIRenderContext& context, float fps, float frameMs)
{
	const float w = static_cast<float>(context.GetClientWidth());
	DrawPanelBackground(context, 0.0f, 0.0f, w, 72.0f);

	std::wstringstream ss;
	ss << L"[DEV OVERLAY] F11 close | FPS " << static_cast<int>(fps + 0.5f)
		<< L"  " << static_cast<int>(frameMs * 10.0f) / 10.0f << L" ms";
	DrawTextLine(context, ss.str(), 12.0f, 8.0f, w - 24.0f);
	DrawTextLine(context,
		L"Left: UI Blocks palette | click=Sprite Shift+click=Button | Tree select | Ins/Ctrl+1..4",
		12.0f,
		28.0f,
		w - 24.0f);
	DrawTextLine(context,
		L"Field edit: click | Tab next | Enter commit | IME CN | Ctrl+S save",
		12.0f,
		48.0f,
		w - 24.0f);
}

void UIEditorRenderer::DrawPanelBackground(UIRenderContext& context, float x, float y, float w, float h)
{
	DrawFilledRect(context.GetD2DTarget(), context.GetBrush(), x, y, w, h,
		D2D1_COLOR_F{ 0.05f, 0.07f, 0.12f, 0.88f });
	DrawRectOutline(context.GetD2DTarget(), context.GetBrush(), x, y, w, h,
		D2D1_COLOR_F{ 0.35f, 0.55f, 0.85f, 0.9f }, 1.0f);
}

void UIEditorRenderer::DrawNodeOutline(UIRenderContext& context, const UINode& node, float absX, float absY, bool selected)
{
	if (!node.IsVisible())
	{
		return;
	}

	const auto& rect = node.GetRect();
	const float x = absX + rect.x;
	const float y = absY + rect.y;
	const D2D1_COLOR_F color = selected
		? D2D1_COLOR_F{ 1.0f, 0.85f, 0.1f, 1.0f }
		: D2D1_COLOR_F{ 0.2f, 0.85f, 1.0f, 0.95f };
	DrawRectOutline(context.GetD2DTarget(), context.GetBrush(), x, y, rect.w, rect.h, color, selected ? 2.5f : 1.0f);
}

void UIEditorRenderer::DrawNodeLabel(UIRenderContext& context, const UINode& node, float absX, float absY, bool selected)
{
	if (!node.IsVisible())
	{
		return;
	}

	const auto& rect = node.GetRect();
	const float x = absX + rect.x;
	const float y = absY + rect.y;
	const std::string typeName = GetNodeTypeName(node);
	std::wstring label = Utf8ToWide(node.GetId()) + L" [" + Utf8ToWide(typeName) + L"]";

	const float labelW = selected ? 220.0f : 180.0f;
	const float labelH = 18.0f;
	const float labelY = y - labelH - 2.0f;
	DrawFilledRect(context.GetD2DTarget(), context.GetBrush(), x, labelY, labelW, labelH,
		D2D1_COLOR_F{ 0.05f, 0.07f, 0.12f, selected ? 0.92f : 0.75f });
	DrawTextLine(context, label, x + 4.0f, labelY + 1.0f, labelW - 8.0f);
}

void UIEditorRenderer::DrawTreePanel(UIRenderContext& context, UIEditorInspector& inspector,
	const UINode& node, float panelX, float& panelY, int depth, const UINode* selected)
{
	const float rowH = UIEditorLayout::kTreeRowH;
	const bool isSelected = &node == selected;

	std::wstring indent(static_cast<size_t>(depth * 2), L' ');
	const std::string typeName = GetNodeTypeName(node);
	std::wstring line = indent + Utf8ToWide(node.GetId());
	line += L"  ";
	line += Utf8ToWide(typeName);
	if (!node.IsVisible())
	{
		line += L" (hidden)";
	}
	if (isSelected)
	{
		line += L"  *";
	}

	if (isSelected)
	{
		DrawFilledRect(context.GetD2DTarget(), context.GetBrush(),
			panelX + 4.0f, panelY - 1.0f, UIEditorLayout::kTreePanelW - 8.0f, rowH,
			D2D1_COLOR_F{ 0.18f, 0.24f, 0.36f, 0.95f });
	}

	DrawTextLine(context, line, panelX + 8.0f, panelY, 280.0f);
	inspector.AddTreeRow(std::const_pointer_cast<UINode>(node.shared_from_this()),
		{ panelX + 4.0f, panelY - 1.0f, UIEditorLayout::kTreePanelW - 8.0f, rowH });
	panelY += rowH;

	for (const auto& child : node.GetChildren())
	{
		DrawTreePanel(context, inspector, *child, panelX, panelY, depth + 1, selected);
	}
}
