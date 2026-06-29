#include "UIEditorSession.h"
#include <algorithm>
#include "UIEditorRenderer.h"
#include "UIEditorLayout.h"
#include "UISystem.h"
#include "UIDocumentLoader.h"
#include "UIPanelNode.h"
#include "UISpriteNode.h"
#include "UITextNode.h"
#include "UIButtonNode.h"
#include "DXTrace.h"
#include "UIEditorTextUtil.h"
#include "UIAtlasRegistry.h"
#include <Windows.h>

using namespace DirectX;

static void DrawAllNodes(UIEditorRenderer& renderer, UIRenderContext& context,
	const UINode& node, float absX, float absY, const UINode* selectedPtr)
{
	const bool isSelected = selectedPtr == &node;
	renderer.DrawNodeOutline(context, node, absX, absY, isSelected);
	renderer.DrawNodeLabel(context, node, absX, absY, isSelected);
	for (const auto& child : node.GetChildren())
	{
		DrawAllNodes(renderer, context, *child, absX + node.GetRect().x, absY + node.GetRect().y, selectedPtr);
	}
}

void UIEditorSession::Attach(UISystem* system, const std::string& sourcePath)
{
	m_system = system;
	m_sourcePath = sourcePath;
}

void UIEditorSession::Detach()
{
	m_system = nullptr;
	m_selectedNode.reset();
	m_active = false;
	ClearFieldFocus();
}

void UIEditorSession::ToggleActive()
{
	m_active = !m_active;
	if (!m_active)
	{
		ClearFieldFocus();
	}
}

void UIEditorSession::SelectNode(const std::shared_ptr<UINode>& node)
{
	m_selectedNode = node;
}

void UIEditorSession::FocusField(const std::shared_ptr<UINode>& node, const std::string& fieldId)
{
	if (!node || fieldId.empty())
	{
		return;
	}
	m_focusedFieldId = fieldId;
	m_inspector.BeginEdit(*node, fieldId, m_editBuffer);
}

void UIEditorSession::ClearFieldFocus()
{
	m_focusedFieldId = {};
	m_editBuffer = {};
}

bool UIEditorSession::CommitFocusedField()
{
	auto selected = GetSelectedNode();
	if (!selected || m_focusedFieldId.empty())
	{
		return false;
	}
	if (!m_inspector.CommitEdit(*selected, m_focusedFieldId, m_editBuffer))
	{
		return false;
	}
	if (auto* renderContext = m_system ? m_system->GetRenderContext() : nullptr)
	{
		selected->Build(*renderContext);
	}
	m_inspector.BeginEdit(*selected, m_focusedFieldId, m_editBuffer);
	return true;
}

namespace
{
char VirtualKeyToAscii(DirectX::Keyboard::Keys key, bool shiftDown)
{
	if (key >= Keyboard::A && key <= Keyboard::Z)
	{
		const char base = shiftDown ? 'A' : 'a';
		return static_cast<char>(base + (static_cast<int>(key) - static_cast<int>(Keyboard::A)));
	}
	if (key >= Keyboard::D0 && key <= Keyboard::D9)
	{
		if (shiftDown)
		{
			const char shifted[] = ")!@#$%^&*(";
			return shifted[static_cast<int>(key) - static_cast<int>(Keyboard::D0)];
		}
		return static_cast<char>('0' + (static_cast<int>(key) - static_cast<int>(Keyboard::D0)));
	}
	switch (key)
	{
	case Keyboard::Space: return ' ';
	case Keyboard::OemMinus: return shiftDown ? '_' : '-';
	case Keyboard::OemPlus: return shiftDown ? '+' : '=';
	case Keyboard::OemPeriod: return shiftDown ? '>' : '.';
	case Keyboard::OemComma: return shiftDown ? '<' : ',';
	case Keyboard::OemQuestion: return shiftDown ? '?' : '/';
	case Keyboard::OemPipe: return shiftDown ? '|' : '\\';
	case Keyboard::OemSemicolon: return shiftDown ? ':' : ';';
	case Keyboard::OemQuotes: return shiftDown ? '"' : '\'';
	case Keyboard::OemTilde: return shiftDown ? '~' : '`';
	default: return '\0';
	}
}

static void AppendPressedAscii(DirectX::Keyboard::KeyboardStateTracker& keyboardTracker,
	const DirectX::Keyboard::State& keyState, std::string& buffer)
{
	const bool shiftDown = keyState.IsKeyDown(Keyboard::LeftShift) || keyState.IsKeyDown(Keyboard::RightShift);
	const Keyboard::Keys keys[] = {
		Keyboard::A, Keyboard::B, Keyboard::C, Keyboard::D, Keyboard::E, Keyboard::F, Keyboard::G, Keyboard::H,
		Keyboard::I, Keyboard::J, Keyboard::K, Keyboard::L, Keyboard::M, Keyboard::N, Keyboard::O, Keyboard::P,
		Keyboard::Q, Keyboard::R, Keyboard::S, Keyboard::T, Keyboard::U, Keyboard::V, Keyboard::W, Keyboard::X,
		Keyboard::Y, Keyboard::Z,
		Keyboard::D0, Keyboard::D1, Keyboard::D2, Keyboard::D3, Keyboard::D4, Keyboard::D5, Keyboard::D6,
		Keyboard::D7, Keyboard::D8, Keyboard::D9,
		Keyboard::Space, Keyboard::OemMinus, Keyboard::OemPlus, Keyboard::OemPeriod, Keyboard::OemComma,
		Keyboard::OemQuestion, Keyboard::OemPipe, Keyboard::OemSemicolon, Keyboard::OemQuotes, Keyboard::OemTilde
	};
	for (const auto key : keys)
	{
		if (!keyboardTracker.IsKeyPressed(key))
		{
			continue;
		}
		const char ch = VirtualKeyToAscii(key, shiftDown);
		if (ch != '\0')
		{
			buffer.push_back(ch);
		}
	}
}
}

void UIEditorSession::HandleFieldEditingKeys(Keyboard::KeyboardStateTracker& keyboardTracker,
	const Keyboard::State& keyState)
{
	if (m_focusedFieldId.empty())
	{
		return;
	}

	auto selected = GetSelectedNode();
	if (!selected)
	{
		ClearFieldFocus();
		return;
	}

	const bool shiftDown = keyState.IsKeyDown(Keyboard::LeftShift) || keyState.IsKeyDown(Keyboard::RightShift);
	const bool isFloatField = IsFloatEditorFieldId(m_focusedFieldId);

	if (keyboardTracker.IsKeyPressed(Keyboard::Back))
	{
		if (isFloatField)
		{
			m_inspector.Backspace(m_editBuffer);
		}
		return;
	}
	if (keyboardTracker.IsKeyPressed(Keyboard::Escape))
	{
		m_inspector.BeginEdit(*selected, m_focusedFieldId, m_editBuffer);
		return;
	}
	if (keyboardTracker.IsKeyPressed(Keyboard::Tab)
		|| keyboardTracker.IsKeyPressed(Keyboard::Enter))
	{
		CommitFocusedField();
		if (keyboardTracker.IsKeyPressed(Keyboard::Tab))
		{
			FocusField(selected, m_inspector.NextFieldId(*selected, m_focusedFieldId));
		}
		return;
	}

	if (isFloatField)
	{
		float step = shiftDown ? 10.0f : 1.0f;
		if (m_focusedFieldId == "scale.x" || m_focusedFieldId == "scale.y" || m_focusedFieldId == "fontSize")
		{
			step = shiftDown ? 0.5f : 0.1f;
		}
		if (keyboardTracker.IsKeyPressed(Keyboard::Up) || keyboardTracker.IsKeyPressed(Keyboard::Down))
		{
			float current = 0.0f;
			try { current = std::stof(m_editBuffer.empty() ? "0" : m_editBuffer); } catch (...) { current = 0.0f; }
			current += keyboardTracker.IsKeyPressed(Keyboard::Up) ? step : -step;
			m_editBuffer = std::to_string(current);
			if (m_editBuffer.find('.') != std::string::npos)
			{
				while (m_editBuffer.size() > 1 && m_editBuffer.back() == '0')
				{
					m_editBuffer.pop_back();
				}
				if (!m_editBuffer.empty() && m_editBuffer.back() == '.')
				{
					m_editBuffer.pop_back();
				}
			}
			CommitFocusedField();
			return;
		}

		AppendPressedAscii(keyboardTracker, keyState, m_editBuffer);
	}
}

void UIEditorSession::OnChar(wchar_t ch)
{
	if (!m_active || m_focusedFieldId.empty())
	{
		return;
	}

	if (IsFloatEditorFieldId(m_focusedFieldId))
	{
		return;
	}

	if (ch == L'\b')
	{
		m_inspector.Backspace(m_editBuffer);
		return;
	}
	if (ch == L'\r' || ch == L'\n' || ch == L'\t')
	{
		return;
	}
	if (ch < 32)
	{
		return;
	}

	m_inspector.AppendUtf8Char(m_editBuffer, ch);
}

void UIEditorSession::HandlePanelClick(UIInputState& input)
{
	const int clientH = m_system && m_system->GetRenderContext()
		? m_system->GetRenderContext()->GetClientHeight() : 0;

	if (UIEditorLayout::IsInToolbar(input.mouseX, input.mouseY))
	{
		return;
	}

	EditorFieldKind fieldKind = EditorFieldKind::Text;
	if (UIEditorLayout::IsInPropertyPanel(input.mouseY, clientH))
	{
		const std::string fieldId = m_inspector.HitTestField(input.mouseX, input.mouseY, &fieldKind);
		if (!fieldId.empty())
		{
			auto selected = GetSelectedNode();
			if (selected)
			{
				if (fieldKind == EditorFieldKind::Bool)
				{
					m_inspector.ToggleBoolField(*selected, fieldId);
				}
				else
				{
					FocusField(selected, fieldId);
				}
			}
		}
		return;
	}

	if (UIEditorLayout::IsInFramePalette(input.mouseX, input.mouseY, clientH))
	{
		std::string atlasId;
		std::string frameName;
		if (m_inspector.HitTestPalette(input.mouseX, input.mouseY, atlasId, frameName))
		{
			const bool asButton = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			ClearFieldFocus();
			AddAtlasNode(atlasId, frameName, asButton);
		}
		return;
	}

	if (UIEditorLayout::IsInTreePanel(input.mouseX, input.mouseY, clientH))
	{
		if (auto node = m_inspector.HitTestTree(input.mouseX, input.mouseY))
		{
			ClearFieldFocus();
			SelectNode(node);
		}
		return;
	}

	if (input.mouseX < UIEditorLayout::kTreePanelW && input.mouseY >= UIEditorLayout::kToolbarH)
	{
		return;
	}

	if (auto hit = m_system->HitTest(input.mouseX, input.mouseY))
	{
		ClearFieldFocus();
		SelectNode(hit);
	}
	else if (!UIEditorLayout::IsInPropertyPanel(input.mouseY, clientH))
	{
		m_selectedNode.reset();
		ClearFieldFocus();
	}
}

void UIEditorSession::Update(UIInputState& input)
{
	if (!m_active || !m_system)
	{
		return;
	}

	if (input.mouseLeftPressed)
	{
		HandlePanelClick(input);
	}
}

void UIEditorSession::MoveSelectedNode(Keyboard::KeyboardStateTracker& keyboardTracker,
	const Keyboard::State& keyState, bool resizeMode)
{
	auto selected = GetSelectedNode();
	if (!selected)
	{
		return;
	}

	const bool fastStep = keyState.IsKeyDown(Keyboard::LeftShift);
	const float step = fastStep ? 10.0f : 1.0f;
	UIRect rect = selected->GetRect();

	if (resizeMode)
	{
		if (keyboardTracker.IsKeyPressed(Keyboard::Left))
		{
			rect.w = (std::max)(1.0f, rect.w - step);
		}
		if (keyboardTracker.IsKeyPressed(Keyboard::Right))
		{
			rect.w += step;
		}
		if (keyboardTracker.IsKeyPressed(Keyboard::Up))
		{
			rect.h = (std::max)(1.0f, rect.h - step);
		}
		if (keyboardTracker.IsKeyPressed(Keyboard::Down))
		{
			rect.h += step;
		}
	}
	else
	{
		if (keyboardTracker.IsKeyPressed(Keyboard::Left))
		{
			rect.x -= step;
		}
		if (keyboardTracker.IsKeyPressed(Keyboard::Right))
		{
			rect.x += step;
		}
		if (keyboardTracker.IsKeyPressed(Keyboard::Up))
		{
			rect.y -= step;
		}
		if (keyboardTracker.IsKeyPressed(Keyboard::Down))
		{
			rect.y += step;
		}
	}

	selected->SetRect(rect);
}

void UIEditorSession::ToggleSelectedVisible()
{
	if (auto selected = GetSelectedNode())
	{
		selected->SetVisible(!selected->IsVisible());
	}
}

void UIEditorSession::HandleEditorKeys(Keyboard::KeyboardStateTracker& keyboardTracker,
	const Keyboard::State& keyState)
{
	if (!m_active || !m_system)
	{
		return;
	}

	if (!m_focusedFieldId.empty())
	{
		HandleFieldEditingKeys(keyboardTracker, keyState);
		return;
	}

	const bool ctrlDown = keyState.IsKeyDown(Keyboard::LeftControl);

	if (keyboardTracker.IsKeyPressed(Keyboard::Insert))
	{
		AddChildNode("Panel");
	}
	if (ctrlDown && keyboardTracker.IsKeyPressed(Keyboard::D1))
	{
		AddChildNode("Panel");
	}
	if (ctrlDown && keyboardTracker.IsKeyPressed(Keyboard::D2))
	{
		AddChildNode("Sprite");
	}
	if (ctrlDown && keyboardTracker.IsKeyPressed(Keyboard::D3))
	{
		AddChildNode("Text");
	}
	if (ctrlDown && keyboardTracker.IsKeyPressed(Keyboard::D4))
	{
		AddChildNode("Button");
	}

	if (keyboardTracker.IsKeyPressed(Keyboard::Delete))
	{
		DeleteSelectedNode();
	}

	if (keyboardTracker.IsKeyPressed(Keyboard::V))
	{
		ToggleSelectedVisible();
	}

	const bool resizeMode = keyState.IsKeyDown(Keyboard::LeftShift);
	MoveSelectedNode(keyboardTracker, keyState, resizeMode);

	if (ctrlDown && keyboardTracker.IsKeyPressed(Keyboard::S))
	{
		SaveDocument();
	}
}

void UIEditorSession::Draw(UIRenderContext& context, float fps, float frameMs)
{
	if (!m_active || !m_system)
	{
		return;
	}

	auto root = m_system->GetRoot();
	if (!root)
	{
		return;
	}

	m_inspector.ClearFrameHits();

	UIEditorRenderer renderer;
	auto selected = GetSelectedNode();
	const UINode* selectedPtr = selected.get();

	auto* target = context.GetD2DTarget();
	if (!target)
	{
		return;
	}

	target->BeginDraw();
	renderer.DrawDimOverlay(context);
	DrawAllNodes(renderer, context, *root, 0.0f, 0.0f, selectedPtr);

	renderer.DrawToolbar(context, fps, frameMs);

	const float paletteTop = UIEditorLayout::GetFramePaletteTop(context.GetClientHeight());
	renderer.DrawPanelBackground(context, 0.0f, UIEditorLayout::kToolbarH,
		UIEditorLayout::kTreePanelW, paletteTop - UIEditorLayout::kToolbarH);
	float treeY = UIEditorLayout::kToolbarH + 10.0f;
	renderer.DrawTreePanel(context, m_inspector, *root, 0.0f, treeY, 0, selectedPtr);

	m_inspector.DrawFramePalette(context, UIEditorLayout::kDefaultAtlasId,
		0.0f, paletteTop, UIEditorLayout::kTreePanelW, UIEditorLayout::kFramePaletteH);

	if (selected)
	{
		const float panelY = static_cast<float>(context.GetClientHeight()) - UIEditorLayout::kPropertyPanelH;
		const float panelW = static_cast<float>(context.GetClientWidth());
		renderer.DrawPanelBackground(context, 0.0f, panelY, panelW, UIEditorLayout::kPropertyPanelH);
		m_inspector.DrawPropertyPanel(context, *selected, 0.0f, panelY, panelW, m_focusedFieldId, m_editBuffer);
	}

	HR(target->EndDraw());
}

void UIEditorSession::AddAtlasNode(const std::string& atlasId, const std::string& frameName, bool asButton)
{
	if (!m_system)
	{
		return;
	}

	auto target = GetSelectedNode();
	if (!target)
	{
		target = m_system->GetRoot();
	}
	if (!target)
	{
		return;
	}

	float w = 160.0f;
	float h = 48.0f;
	if (const UIAtlasFrame* frame = UIAtlasRegistry::Instance().GetFrameDefinition(atlasId, frameName))
	{
		if (frame->w > 0.0f)
		{
			w = frame->w;
		}
		if (frame->h > 0.0f)
		{
			h = frame->h;
		}
	}

	auto* renderContext = m_system->GetRenderContext();
	const int idNum = ++m_nodeCounter;
	std::shared_ptr<UINode> node;

	if (asButton)
	{
		auto button = std::make_shared<UIButtonNode>();
		button->SetId("btn_" + std::to_string(idNum));
		button->SetAtlasId(atlasId);
		button->SetFrameName(frameName);
		button->SetLabelUtf8("Button");
		button->SetRect({ 10.0f, 10.0f, w, h });
		node = button;
	}
	else
	{
		auto sprite = std::make_shared<UISpriteNode>();
		sprite->SetId("sprite_" + std::to_string(idNum));
		sprite->SetAtlasId(atlasId);
		sprite->SetFrameName(frameName);
		sprite->SetRect({ 10.0f, 10.0f, w, h });
		node = sprite;
	}

	target->AddChild(node);
	if (renderContext)
	{
		node->Build(*renderContext);
	}
	ClearFieldFocus();
	SelectNode(node);
}

void UIEditorSession::AddChildNode(const std::string& nodeType)
{
	if (!m_system)
	{
		return;
	}

	auto target = GetSelectedNode();
	if (!target)
	{
		target = m_system->GetRoot();
	}
	if (!target)
	{
		return;
	}

	auto* renderContext = m_system->GetRenderContext();
	const int idNum = ++m_nodeCounter;
	std::shared_ptr<UINode> node;

	if (nodeType == "Sprite")
	{
		AddAtlasNode(UIEditorLayout::kDefaultAtlasId, "window_line", false);
		return;
	}
	else if (nodeType == "Text")
	{
		auto text = std::make_shared<UITextNode>();
		text->SetId("text_" + std::to_string(idNum));
		text->SetTextUtf8("New Text");
		text->SetFontSize(24.0f);
		text->SetRect({ 10.0f, 10.0f, 200.0f, 32.0f });
		node = text;
	}
	else if (nodeType == "Button")
	{
		AddAtlasNode(UIEditorLayout::kDefaultAtlasId, "window_line", true);
		return;
	}
	else
	{
		auto panel = std::make_shared<UIPanelNode>();
		panel->SetId("panel_" + std::to_string(idNum));
		panel->SetRect({ 10.0f, 10.0f, 120.0f, 48.0f });
		node = panel;
	}

	target->AddChild(node);
	if (renderContext)
	{
		node->Build(*renderContext);
	}
	ClearFieldFocus();
	SelectNode(node);
}

bool UIEditorSession::DeleteSelectedNode()
{
	if (!m_system)
	{
		return false;
	}

	auto selected = GetSelectedNode();
	auto root = m_system->GetRoot();
	if (!selected || !root || selected.get() == root.get())
	{
		return false;
	}

	UINode* parent = selected->GetParent();
	if (!parent)
	{
		return false;
	}

	parent->RemoveChild(selected);
	m_selectedNode.reset();
	ClearFieldFocus();
	return true;
}

bool UIEditorSession::SaveDocument()
{
	if (!m_system)
	{
		return false;
	}
	if (!m_focusedFieldId.empty())
	{
		CommitFocusedField();
	}
	auto document = m_system->GetDocument();
	if (!document)
	{
		return false;
	}
	const std::string path = m_sourcePath.empty() ? document->sourcePath : m_sourcePath;
	if (!UIDocumentLoader::SaveToFile(*document, path))
	{
		return false;
	}

	m_system->ReloadDocument();
	m_selectedNode.reset();
	ClearFieldFocus();
	return true;
}
