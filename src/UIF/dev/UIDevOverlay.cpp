#include "UIDevOverlay.h"
#include "UIEditorSession.h"
#include "UIEditorRenderer.h"
#include "UISystem.h"
#include "logger_manager.h"

UIDevOverlay& UIDevOverlay::Instance()
{
	static UIDevOverlay instance;
	return instance;
}

UIDevOverlay::UIDevOverlay()
	: m_uiEditor(std::make_unique<UIEditorSession>())
{
}

void UIDevOverlay::BindUISystem(UISystem* system, const std::string& documentPath)
{
	m_uiEditor->Attach(system, documentPath);
}

void UIDevOverlay::Unbind()
{
	m_uiEditor->Detach();
}

bool UIDevOverlay::IsActive() const
{
	return m_uiEditor && m_uiEditor->IsActive();
}

void UIDevOverlay::Toggle()
{
	if (!m_uiEditor)
	{
		return;
	}
	m_uiEditor->ToggleActive();
	INFO_("UIDevOverlay: {}", m_uiEditor->IsActive() ? "开启 (F11 关闭)" : "关闭");
}

void UIDevOverlay::SetStats(float fps, float frameMs)
{
	m_fps = fps;
	m_frameMs = frameMs;
}

void UIDevOverlay::HandleGlobalKeys(DirectX::Keyboard::KeyboardStateTracker& keyboardTracker)
{
	if (keyboardTracker.IsKeyPressed(DirectX::Keyboard::F11))
	{
		Toggle();
	}
}

void UIDevOverlay::HandleEditorKeys(DirectX::Keyboard::KeyboardStateTracker& keyboardTracker,
	const DirectX::Keyboard::State& keyState)
{
	if (!m_uiEditor || !m_uiEditor->IsActive())
	{
		return;
	}
	m_uiEditor->HandleEditorKeys(keyboardTracker, keyState);
}

bool UIDevOverlay::OnChar(wchar_t ch)
{
	if (!m_uiEditor || !m_uiEditor->IsActive())
	{
		return false;
	}
	m_uiEditor->OnChar(ch);
	return true;
}

void UIDevOverlay::UpdateInput(UIInputState& input)
{
	if (!m_uiEditor || !m_uiEditor->IsActive())
	{
		return;
	}
	m_uiEditor->Update(input);
}

void UIDevOverlay::Draw(UIRenderContext& context)
{
	if (!m_uiEditor || !m_uiEditor->IsActive())
	{
		return;
	}
	m_uiEditor->Draw(context, m_fps, m_frameMs);
}
