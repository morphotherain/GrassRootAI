#pragma once

#include "UIDocument.h"
#include <memory>

class UIRenderContext;
struct UIInputState;

class UISystem
{
public:
	void SetRenderContext(UIRenderContext* context) { m_context = context; }
	UIRenderContext* GetRenderContext() const { return m_context; }

	void SetDocument(std::shared_ptr<UIDocument> document);
	bool ReloadDocument();
	std::shared_ptr<UIDocument> GetDocument() const { return m_document; }
	std::shared_ptr<UINode> GetRoot() const;

	bool Update(UIInputState& input);
	void Draw();

	std::shared_ptr<UINode> HitTest(float x, float y);

private:
	UIRenderContext* m_context = nullptr;
	std::shared_ptr<UIDocument> m_document;
};
