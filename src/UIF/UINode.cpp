#include "UINode.h"

void UINode::AddChild(std::shared_ptr<UINode> child)
{
	if (!child)
	{
		return;
	}
	child->m_parent = this;
	m_children.push_back(std::move(child));
}

void UINode::RemoveChild(const std::shared_ptr<UINode>& child)
{
	for (auto it = m_children.begin(); it != m_children.end(); ++it)
	{
		if (*it == child)
		{
			(*it)->m_parent = nullptr;
			m_children.erase(it);
			return;
		}
	}
}

void UINode::ClearChildren()
{
	for (auto& child : m_children)
	{
		if (child)
		{
			child->m_parent = nullptr;
		}
	}
	m_children.clear();
}

std::shared_ptr<UINode> UINode::FindById(const std::string& id)
{
	if (m_id == id)
	{
		return shared_from_this();
	}
	for (auto& child : m_children)
	{
		if (auto found = child->FindById(id))
		{
			return found;
		}
	}
	return nullptr;
}

void UINode::Build(UIRenderContext& context)
{
	for (auto& child : m_children)
	{
		child->Build(context);
	}
}

void UINode::Measure(UIRenderContext& context)
{
	for (auto& child : m_children)
	{
		child->Measure(context);
	}
}

void UINode::Arrange(UIRenderContext& context, float offsetX, float offsetY)
{
	for (auto& child : m_children)
	{
		child->Arrange(context, offsetX + m_rect.x, offsetY + m_rect.y);
	}
}

bool UINode::Update(UIInputState& input, UIRenderContext& context, float absX, float absY)
{
	if (!m_visible)
	{
		return false;
	}

	bool consumed = false;
	for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
	{
		if ((*it)->Update(input, context, absX + m_rect.x, absY + m_rect.y))
		{
			consumed = true;
			break;
		}
	}
	return consumed;
}

void UINode::Draw(UIRenderContext& context, float absX, float absY)
{
	if (!m_visible)
	{
		return;
	}
	for (auto& child : m_children)
	{
		child->Draw(context, absX + m_rect.x, absY + m_rect.y);
	}
}

std::shared_ptr<UINode> UINode::HitTest(float px, float py, float nodeAbsX, float nodeAbsY)
{
	if (!m_visible)
	{
		return nullptr;
	}

	const float localX = px - nodeAbsX;
	const float localY = py - nodeAbsY;

	for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
	{
		if (auto hit = (*it)->HitTest(px, py, nodeAbsX + m_rect.x, nodeAbsY + m_rect.y))
		{
			return hit;
		}
	}

	if (m_rect.Contains(localX, localY))
	{
		return shared_from_this();
	}
	return nullptr;
}
