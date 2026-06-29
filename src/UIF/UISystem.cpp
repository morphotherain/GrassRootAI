#include "UISystem.h"
#include "UIRenderContext.h"
#include "UIDocumentLoader.h"

void UISystem::SetDocument(std::shared_ptr<UIDocument> document)
{
	m_document = std::move(document);
	if (m_document && m_document->root && m_context)
	{
		m_document->root->Build(*m_context);
		m_document->root->Measure(*m_context);
		m_document->root->Arrange(*m_context, 0.0f, 0.0f);
	}
}

bool UISystem::ReloadDocument()
{
	if (!m_document || m_document->sourcePath.empty())
	{
		return false;
	}
	auto document = UIDocumentLoader::LoadFromFile(m_document->sourcePath);
	if (!document || !document->root)
	{
		return false;
	}
	SetDocument(document);
	return true;
}

std::shared_ptr<UINode> UISystem::GetRoot() const
{
	return m_document ? m_document->root : nullptr;
}

bool UISystem::Update(UIInputState& input)
{
	if (!m_document || !m_document->root)
	{
		return false;
	}
	return m_document->root->Update(input, *m_context, 0.0f, 0.0f);
}

void UISystem::Draw()
{
	if (!m_context || !m_document || !m_document->root)
	{
		return;
	}

	m_context->ClearFrame();
	m_document->root->Draw(*m_context, 0.0f, 0.0f);
}

std::shared_ptr<UINode> UISystem::HitTest(float x, float y)
{
	if (!m_document || !m_document->root)
	{
		return nullptr;
	}
	return m_document->root->HitTest(x, y, 0.0f, 0.0f);
}
