#include "UIEditorInspector.h"
#include "UIEditorRenderer.h"
#include "UIEditorLayout.h"
#include "UIEditorTextUtil.h"
#include "UIAtlasRegistry.h"
#include "UIButtonNode.h"
#include "UISpriteNode.h"
#include "UITextNode.h"
#include <Windows.h>
#include <cctype>
#include <cmath>
#include <sstream>

namespace
{
	std::string FloatToString(float value)
	{
		std::ostringstream ss;
		ss.precision(4);
		ss << std::fixed << value;
		std::string text = ss.str();
		while (text.size() > 1 && text.back() == '0' && text.find('.') != std::string::npos)
		{
			text.pop_back();
		}
		if (!text.empty() && text.back() == '.')
		{
			text.pop_back();
		}
		return text;
	}

	bool TryParseFloat(const std::string& text, float& outValue)
	{
		if (text.empty())
		{
			return false;
		}
		try
		{
			size_t idx = 0;
			const float value = std::stof(text, &idx);
			while (idx < text.size() && std::isspace(static_cast<unsigned char>(text[idx])))
			{
				++idx;
			}
			if (idx != text.size())
			{
				return false;
			}
			outValue = value;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void DrawFieldBox(UIRenderContext& context, const UIRect& bounds, const std::wstring& value, bool focused)
	{
		auto* target = context.GetD2DTarget();
		auto* brush = context.GetBrush();
		auto* format = context.GetTextFormat();
		if (!target || !brush || !format)
		{
			return;
		}

		brush->SetColor(focused
			? D2D1_COLOR_F{ 0.12f, 0.18f, 0.28f, 0.95f }
			: D2D1_COLOR_F{ 0.08f, 0.10f, 0.14f, 0.92f });
		target->FillRectangle(D2D1_RECT_F{ bounds.x, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h }, brush);

		brush->SetColor(focused
			? D2D1_COLOR_F{ 1.0f, 0.85f, 0.1f, 1.0f }
			: D2D1_COLOR_F{ 0.35f, 0.55f, 0.85f, 0.85f });
		target->DrawRectangle(D2D1_RECT_F{ bounds.x, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h }, brush, focused ? 2.0f : 1.0f);

		brush->SetColor(D2D1_COLOR_F{ 0.92f, 0.94f, 0.98f, 1.0f });
		target->DrawTextW(
			value.c_str(),
			static_cast<UINT32>(value.size()),
			format,
			D2D1_RECT_F{ bounds.x + 4.0f, bounds.y + 2.0f, bounds.x + bounds.w - 4.0f, bounds.y + bounds.h - 2.0f },
			brush);
	}

	void DrawLabel(UIRenderContext& context, const std::wstring& label, float x, float y)
	{
		auto* target = context.GetD2DTarget();
		auto* brush = context.GetBrush();
		auto* format = context.GetTextFormat();
		if (!target || !brush || !format)
		{
			return;
		}
		brush->SetColor(D2D1_COLOR_F{ 0.75f, 0.82f, 0.92f, 1.0f });
		target->DrawTextW(
			label.c_str(),
			static_cast<UINT32>(label.size()),
			format,
			D2D1_RECT_F{ x, y, x + 72.0f, y + 18.0f },
			brush);
	}
}

void UIEditorInspector::ClearFrameHits()
{
	m_treeRows.clear();
	m_fieldHits.clear();
	m_paletteRows.clear();
}

void UIEditorInspector::AddTreeRow(const std::shared_ptr<UINode>& node, const UIRect& bounds)
{
	if (!node)
	{
		return;
	}
	m_treeRows.push_back({ node, bounds });
}

std::shared_ptr<UINode> UIEditorInspector::HitTestTree(float mouseX, float mouseY) const
{
	for (const auto& row : m_treeRows)
	{
		if (row.bounds.Contains(mouseX, mouseY))
		{
			return row.node.lock();
		}
	}
	return nullptr;
}

void UIEditorInspector::AddPaletteRow(const std::string& atlasId, const std::string& frameName, const UIRect& bounds) const
{
	m_paletteRows.push_back({ atlasId, frameName, bounds });
}

bool UIEditorInspector::HitTestPalette(float mouseX, float mouseY, std::string& outAtlasId, std::string& outFrameName) const
{
	for (const auto& row : m_paletteRows)
	{
		if (row.bounds.Contains(mouseX, mouseY))
		{
			outAtlasId = row.atlasId;
			outFrameName = row.frameName;
			return true;
		}
	}
	return false;
}

float UIEditorInspector::DrawFramePalette(UIRenderContext& context, const std::string& atlasId,
	float panelX, float panelTop, float panelW, float panelH) const
{
	auto* target = context.GetD2DTarget();
	auto* brush = context.GetBrush();
	auto* format = context.GetTextFormat();
	if (!target || !brush || !format)
	{
		return panelTop;
	}

	brush->SetColor(D2D1_COLOR_F{ 0.05f, 0.07f, 0.12f, 0.92f });
	target->FillRectangle(D2D1_RECT_F{ panelX, panelTop, panelX + panelW, panelTop + panelH }, brush);
	brush->SetColor(D2D1_COLOR_F{ 0.35f, 0.55f, 0.85f, 0.9f });
	target->DrawRectangle(D2D1_RECT_F{ panelX, panelTop, panelX + panelW, panelTop + panelH }, brush, 1.0f);

	brush->SetColor(D2D1_COLOR_F{ 0.75f, 0.82f, 0.92f, 1.0f });
	const std::wstring title = L"UI Blocks (" + Utf8ToWide(atlasId) + L") click=Sprite Shift=Button";
	target->DrawTextW(
		title.c_str(),
		static_cast<UINT32>(title.size()),
		format,
		D2D1_RECT_F{ panelX + 8.0f, panelTop + 4.0f, panelX + panelW - 8.0f, panelTop + 22.0f },
		brush);

	const std::vector<std::string> frameNames = UIAtlasRegistry::Instance().GetFrameNames(atlasId);
	const float rowH = 18.0f;
	const float colW = (panelW - 16.0f) * 0.5f;
	const float listTop = panelTop + 24.0f;

	for (size_t i = 0; i < frameNames.size(); ++i)
	{
		const int col = static_cast<int>(i % 2);
		const int row = static_cast<int>(i / 2);
		const float x = panelX + 8.0f + static_cast<float>(col) * colW;
		const float y = listTop + static_cast<float>(row) * rowH;
		if (y + rowH > panelTop + panelH - 4.0f)
		{
			break;
		}

		brush->SetColor(D2D1_COLOR_F{ 0.88f, 0.90f, 0.96f, 1.0f });
		const std::wstring frameLabel = Utf8ToWide(frameNames[i]);
		target->DrawTextW(
			frameLabel.c_str(),
			static_cast<UINT32>(frameLabel.size()),
			format,
			D2D1_RECT_F{ x, y, x + colW - 4.0f, y + rowH },
			brush);

		AddPaletteRow(atlasId, frameNames[i], { x, y, colW - 4.0f, rowH });
	}

	return panelTop + panelH;
}

void UIEditorInspector::BuildFieldSpecs(const UINode& node, std::vector<EditorFieldSpec>& outSpecs) const
{
	outSpecs.clear();
	outSpecs.push_back({ "id", "id", EditorFieldKind::Text });
	outSpecs.push_back({ "rect.x", "x", EditorFieldKind::Float });
	outSpecs.push_back({ "rect.y", "y", EditorFieldKind::Float });
	outSpecs.push_back({ "rect.w", "w", EditorFieldKind::Float });
	outSpecs.push_back({ "rect.h", "h", EditorFieldKind::Float });
	outSpecs.push_back({ "visible", "visible", EditorFieldKind::Bool });
	outSpecs.push_back({ "scale.x", "scaleX", EditorFieldKind::Float });
	outSpecs.push_back({ "scale.y", "scaleY", EditorFieldKind::Float });
	outSpecs.push_back({ "rotation", "rot", EditorFieldKind::Float });

	if (dynamic_cast<const UISpriteNode*>(&node) && !dynamic_cast<const UIButtonNode*>(&node))
	{
		outSpecs.push_back({ "atlas", "atlas", EditorFieldKind::Text });
		outSpecs.push_back({ "frame", "frame", EditorFieldKind::Text });
		outSpecs.push_back({ "texture", "texture", EditorFieldKind::Text });
	}
	if (dynamic_cast<const UITextNode*>(&node))
	{
		outSpecs.push_back({ "text", "text", EditorFieldKind::Text });
		outSpecs.push_back({ "fontSize", "font", EditorFieldKind::Float });
	}
	if (dynamic_cast<const UIButtonNode*>(&node))
	{
		outSpecs.push_back({ "atlas", "atlas", EditorFieldKind::Text });
		outSpecs.push_back({ "frame", "frame", EditorFieldKind::Text });
		outSpecs.push_back({ "texture", "texture", EditorFieldKind::Text });
		outSpecs.push_back({ "text", "label", EditorFieldKind::Text });
		outSpecs.push_back({ "action", "action", EditorFieldKind::Text });
	}
}

float UIEditorInspector::DrawPropertyPanel(UIRenderContext& context, const UINode& node,
	float panelX, float panelY, float panelW,
	const std::string& focusedFieldId, const std::string& editBuffer) const
{
	std::vector<EditorFieldSpec> specs;
	BuildFieldSpecs(node, specs);

	const float labelW = 72.0f;
	const float fieldX = panelX + labelW + 8.0f;
	const float fieldW = panelW - labelW - 20.0f;
	const float rowH = 22.0f;
	float rowY = panelY + 8.0f;

	DrawLabel(context, L"Inspector", panelX + 8.0f, rowY);
	rowY += rowH;

	for (const auto& spec : specs)
	{
		DrawLabel(context, Utf8ToWide(spec.label), panelX + 8.0f, rowY + 2.0f);

		const bool focused = spec.id == focusedFieldId;
		const std::string displayUtf8 = focused ? editBuffer : GetFieldDisplayValueUtf8(node, spec.id);
		const UIRect bounds{ fieldX, rowY, fieldW, rowH - 2.0f };
		DrawFieldBox(context, bounds, Utf8ToWide(displayUtf8), focused);
		m_fieldHits.push_back({ spec.id, spec.kind, bounds });
		rowY += rowH;
	}

	return rowY - panelY + 8.0f;
}

std::string UIEditorInspector::HitTestField(float mouseX, float mouseY, EditorFieldKind* outKind) const
{
	for (const auto& field : m_fieldHits)
	{
		if (field.bounds.Contains(mouseX, mouseY))
		{
			if (outKind)
			{
				*outKind = field.kind;
			}
			return field.id;
		}
	}
	return {};
}

std::string UIEditorInspector::GetFieldDisplayValueUtf8(const UINode& node, const std::string& fieldId)
{
	const UIRect& rect = node.GetRect();
	if (fieldId == "id") return node.GetId();
	if (fieldId == "rect.x") return FloatToString(rect.x);
	if (fieldId == "rect.y") return FloatToString(rect.y);
	if (fieldId == "rect.w") return FloatToString(rect.w);
	if (fieldId == "rect.h") return FloatToString(rect.h);
	if (fieldId == "visible") return node.IsVisible() ? "true" : "false";
	if (fieldId == "scale.x") return FloatToString(node.GetScaleX());
	if (fieldId == "scale.y") return FloatToString(node.GetScaleY());
	if (fieldId == "rotation") return FloatToString(node.GetRotation());

	if (const auto* sprite = dynamic_cast<const UISpriteNode*>(&node))
	{
		if (fieldId == "atlas") return sprite->GetAtlasId();
		if (fieldId == "frame") return sprite->GetFrameName();
		if (fieldId == "texture") return sprite->GetTexturePath();
	}
	if (const auto* text = dynamic_cast<const UITextNode*>(&node))
	{
		if (fieldId == "text") return WideToUtf8(text->GetText());
		if (fieldId == "fontSize") return FloatToString(text->GetFontSize());
	}
	if (const auto* button = dynamic_cast<const UIButtonNode*>(&node))
	{
		if (fieldId == "atlas") return button->GetAtlasId();
		if (fieldId == "frame") return button->GetFrameName();
		if (fieldId == "texture") return button->GetTexturePath();
		if (fieldId == "text") return WideToUtf8(button->GetLabel());
		if (fieldId == "action") return button->GetActionId();
	}
	return {};
}

void UIEditorInspector::BeginEdit(const UINode& node, const std::string& fieldId, std::string& outBuffer) const
{
	outBuffer = GetFieldDisplayValueUtf8(node, fieldId);
}

bool UIEditorInspector::CommitEdit(UINode& node, const std::string& fieldId, const std::string& buffer) const
{
	UIRect rect = node.GetRect();
	if (fieldId == "id")
	{
		if (buffer.empty())
		{
			return false;
		}
		node.SetId(buffer);
		return true;
	}
	if (fieldId == "rect.x") { float v; if (!TryParseFloat(buffer, v)) return false; rect.x = v; node.SetRect(rect); return true; }
	if (fieldId == "rect.y") { float v; if (!TryParseFloat(buffer, v)) return false; rect.y = v; node.SetRect(rect); return true; }
	if (fieldId == "rect.w") { float v; if (!TryParseFloat(buffer, v)) return false; rect.w = (v > 0.0f) ? v : 1.0f; node.SetRect(rect); return true; }
	if (fieldId == "rect.h") { float v; if (!TryParseFloat(buffer, v)) return false; rect.h = (v > 0.0f) ? v : 1.0f; node.SetRect(rect); return true; }
	if (fieldId == "scale.x") { float v; if (!TryParseFloat(buffer, v)) return false; node.SetScaleX(v); return true; }
	if (fieldId == "scale.y") { float v; if (!TryParseFloat(buffer, v)) return false; node.SetScaleY(v); return true; }
	if (fieldId == "rotation") { float v; if (!TryParseFloat(buffer, v)) return false; node.SetRotation(v); return true; }

	if (auto* text = dynamic_cast<UITextNode*>(&node))
	{
		if (fieldId == "text") { text->SetTextUtf8(buffer); return true; }
		if (fieldId == "fontSize") { float v; if (!TryParseFloat(buffer, v)) return false; text->SetFontSize(v); return true; }
	}
	if (auto* sprite = dynamic_cast<UISpriteNode*>(&node))
	{
		if (fieldId == "atlas") { sprite->SetAtlasId(buffer); return true; }
		if (fieldId == "frame") { sprite->SetFrameName(buffer); return true; }
		if (fieldId == "texture")
		{
			sprite->SetTexturePath(buffer);
			return true;
		}
	}
	if (auto* button = dynamic_cast<UIButtonNode*>(&node))
	{
		if (fieldId == "atlas") { button->SetAtlasId(buffer); return true; }
		if (fieldId == "frame") { button->SetFrameName(buffer); return true; }
		if (fieldId == "texture") { button->SetTexturePath(buffer); return true; }
		if (fieldId == "text") { button->SetLabelUtf8(buffer); return true; }
		if (fieldId == "action") { button->SetActionId(buffer); return true; }
	}
	return false;
}

bool UIEditorInspector::ToggleBoolField(UINode& node, const std::string& fieldId) const
{
	if (fieldId == "visible")
	{
		node.SetVisible(!node.IsVisible());
		return true;
	}
	return false;
}

void UIEditorInspector::AppendUtf8Char(std::string& buffer, wchar_t ch) const
{
	if (ch == L'\b' || ch == L'\r' || ch == L'\n' || ch == L'\t')
	{
		return;
	}
	const int size = WideCharToMultiByte(CP_UTF8, 0, &ch, 1, nullptr, 0, nullptr, nullptr);
	if (size <= 0)
	{
		return;
	}
	std::string utf8(size, '\0');
	WideCharToMultiByte(CP_UTF8, 0, &ch, 1, utf8.data(), size, nullptr, nullptr);
	buffer += utf8;
}

void UIEditorInspector::Backspace(std::string& buffer) const
{
	if (buffer.empty())
	{
		return;
	}
	while (!buffer.empty())
	{
		const unsigned char tail = static_cast<unsigned char>(buffer.back());
		buffer.pop_back();
		if ((tail & 0xC0) != 0x80)
		{
			break;
		}
	}
}

std::string UIEditorInspector::NextFieldId(const UINode& node, const std::string& currentFieldId) const
{
	std::vector<EditorFieldSpec> specs;
	BuildFieldSpecs(node, specs);
	if (specs.empty())
	{
		return {};
	}
	if (currentFieldId.empty())
	{
		return specs.front().id;
	}
	for (size_t i = 0; i < specs.size(); ++i)
	{
		if (specs[i].id == currentFieldId)
		{
			return specs[(i + 1) % specs.size()].id;
		}
	}
	return specs.front().id;
}
