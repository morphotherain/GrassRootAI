#include "UINavigationStack.h"
#include "UIDocumentLoader.h"
#include "UIDevOverlay.h"
#include "UIActionRegistry.h"
#include "UIPanelNode.h"
#include "UISystem.h"
#include "logger_manager.h"

namespace
{
	constexpr float kDesignWidth = 1920.0f;
	constexpr float kDesignHeight = 1080.0f;

	void ScaleNodeTree(UINode& node, float scaleX, float scaleY)
	{
		UIRect rect = node.GetRect();
		rect.x *= scaleX;
		rect.y *= scaleY;
		rect.w *= scaleX;
		rect.h *= scaleY;
		node.SetRect(rect);

		for (const auto& child : node.GetChildren())
		{
			if (child)
			{
				ScaleNodeTree(*child, scaleX, scaleY);
			}
		}
	}

	void FitRootToClient(UINode& root, float clientWidth, float clientHeight)
	{
		root.SetRect({ 0.0f, 0.0f, clientWidth, clientHeight });
		if (auto bg = root.FindById("bg"))
		{
			bg->SetRect({ 0.0f, 0.0f, clientWidth, clientHeight });
		}
	}
}

void UINavigationStack::Initialize(UISystem* system, UIRenderContext* context, int clientWidth, int clientHeight)
{
	m_system = system;
	m_context = context;
	m_clientWidth = clientWidth;
	m_clientHeight = clientHeight;
}

void UINavigationStack::Shutdown()
{
	while (!m_stack.empty())
	{
		if (m_stack.back().controller)
		{
			m_stack.back().controller->OnHide();
		}
		m_stack.pop_back();
	}
	UIActionRegistry::Instance().Unregister("nav.back");
	m_busy = false;
}

void UINavigationStack::ReplaceRoot(const std::string& documentPath, std::unique_ptr<IUIScreenController> controller)
{
	Shutdown();
	if (!m_system || !m_context)
	{
		return;
	}

	ScreenEntry entry;
	entry.documentPath = documentPath;
	entry.document = LoadScaledDocument(documentPath);
	entry.controller = std::move(controller);
	if (!entry.document || !entry.document->root)
	{
		ERROR_("UINavigationStack: 无法加载屏幕 {}", documentPath);
		return;
	}

	m_stack.push_back(std::move(entry));
	ApplyTopScreen();
}

void UINavigationStack::Push(const std::string& documentPath, std::unique_ptr<IUIScreenController> controller)
{
	if (!m_system || !m_context)
	{
		return;
	}
	if (!m_stack.empty() && m_stack.back().controller)
	{
		m_stack.back().controller->OnHide();
	}

	ScreenEntry entry;
	entry.documentPath = documentPath;
	entry.document = LoadScaledDocument(documentPath);
	entry.controller = std::move(controller);
	if (!entry.document || !entry.document->root)
	{
		ERROR_("UINavigationStack: 无法加载屏幕 {}", documentPath);
		if (!m_stack.empty() && m_stack.back().controller)
		{
			m_stack.back().controller->OnShow(*m_system, *m_context);
		}
		return;
	}

	m_stack.push_back(std::move(entry));
	ApplyTopScreen();
}

bool UINavigationStack::Pop()
{
	if (m_stack.size() <= 1 || !m_system || !m_context)
	{
		return false;
	}

	if (m_stack.back().controller)
	{
		m_stack.back().controller->OnHide();
	}
	m_stack.pop_back();
	ApplyTopScreen();
	return true;
}

bool UINavigationStack::HandleBack()
{
	if (m_busy)
	{
		return true;
	}
	return Pop();
}

void UINavigationStack::Update(UIInputState& input)
{
	if (m_busy || m_stack.empty() || !m_stack.back().controller)
	{
		return;
	}
	m_stack.back().controller->OnUpdate(input);
}

bool UINavigationStack::OnChar(wchar_t ch)
{
	if (m_busy || m_stack.empty() || !m_stack.back().controller)
	{
		return false;
	}
	return m_stack.back().controller->OnChar(ch);
}

void UINavigationStack::SetBusy(bool busy)
{
	m_busy = busy;
	UpdateBusyOverlay();
}

const std::string& UINavigationStack::TopDocumentPath() const
{
	static const std::string empty;
	if (m_stack.empty())
	{
		return empty;
	}
	return m_stack.back().documentPath;
}

void UINavigationStack::OnClientResize(int clientWidth, int clientHeight)
{
	m_clientWidth = clientWidth;
	m_clientHeight = clientHeight;
	if (m_stack.empty())
	{
		return;
	}

	for (auto& entry : m_stack)
	{
		entry.document = UIDocumentLoader::LoadFromFile(entry.documentPath);
		if (entry.document && entry.document->root)
		{
			const float scaleX = static_cast<float>(m_clientWidth) / kDesignWidth;
			const float scaleY = static_cast<float>(m_clientHeight) / kDesignHeight;
			ScaleNodeTree(*entry.document->root, scaleX, scaleY);
			FitRootToClient(*entry.document->root, static_cast<float>(m_clientWidth), static_cast<float>(m_clientHeight));
		}
	}
	ApplyTopScreen();
}

std::shared_ptr<UIDocument> UINavigationStack::LoadScaledDocument(const std::string& path)
{
	auto document = UIDocumentLoader::LoadFromFile(path);
	if (!document || !document->root)
	{
		return document;
	}

	const float scaleX = static_cast<float>(m_clientWidth) / kDesignWidth;
	const float scaleY = static_cast<float>(m_clientHeight) / kDesignHeight;
	ScaleNodeTree(*document->root, scaleX, scaleY);
	FitRootToClient(*document->root, static_cast<float>(m_clientWidth), static_cast<float>(m_clientHeight));
	return document;
}

void UINavigationStack::ApplyTopScreen()
{
	if (m_stack.empty() || !m_system || !m_context)
	{
		return;
	}

	auto& top = m_stack.back();
	m_system->SetDocument(top.document);
	RegisterNavigationActions();
	SetOverlayPath(top.documentPath);
	UpdateBusyOverlay();

	if (top.controller)
	{
		top.controller->OnShow(*m_system, *m_context);
	}
}

void UINavigationStack::RegisterNavigationActions()
{
	UIActionRegistry::Instance().Register("nav.back", [this]() {
		Pop();
	});
}

void UINavigationStack::SetOverlayPath(const std::string& path)
{
	UIDevOverlay::Instance().BindUISystem(m_system, path);
}

void UINavigationStack::UpdateBusyOverlay()
{
	if (m_stack.empty() || !m_stack.back().document || !m_stack.back().document->root)
	{
		return;
	}
	if (auto overlay = m_stack.back().document->root->FindById("loading_overlay"))
	{
		overlay->SetVisible(m_busy);
	}
}
