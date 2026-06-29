#pragma once

#include "UIRect.h"
#include "UIRenderContext.h"
#include "UIInputState.h"
#include <memory>
#include <string>
#include <vector>

class UINode : public std::enable_shared_from_this<UINode>
{
public:
	virtual ~UINode() = default;

	const std::string& GetId() const { return m_id; }
	void SetId(const std::string& id) { m_id = id; }

	const UIRect& GetRect() const { return m_rect; }
	void SetRect(const UIRect& rect) { m_rect = rect; }

	bool IsVisible() const { return m_visible; }
	void SetVisible(bool visible) { m_visible = visible; }

	float GetScaleX() const { return m_scaleX; }
	float GetScaleY() const { return m_scaleY; }
	float GetRotation() const { return m_rotation; }
	void SetScaleX(float scaleX) { m_scaleX = scaleX; }
	void SetScaleY(float scaleY) { m_scaleY = scaleY; }
	void SetRotation(float rotation) { m_rotation = rotation; }

	UINode* GetParent() const { return m_parent; }
	const std::vector<std::shared_ptr<UINode>>& GetChildren() const { return m_children; }

	void AddChild(std::shared_ptr<UINode> child);
	void RemoveChild(const std::shared_ptr<UINode>& child);
	void ClearChildren();

	std::shared_ptr<UINode> FindById(const std::string& id);

	virtual void Build(UIRenderContext& context);
	virtual void Measure(UIRenderContext& context);
	virtual void Arrange(UIRenderContext& context, float offsetX, float offsetY);

	virtual bool Update(UIInputState& input, UIRenderContext& context, float absX, float absY);
	virtual void Draw(UIRenderContext& context, float absX, float absY);

	std::shared_ptr<UINode> HitTest(float absX, float absY, float nodeAbsX, float nodeAbsY);

protected:
	std::string m_id;
	UIRect m_rect;
	bool m_visible = true;
	float m_scaleX = 1.0f;
	float m_scaleY = 1.0f;
	float m_rotation = 0.0f;
	UINode* m_parent = nullptr;
	std::vector<std::shared_ptr<UINode>> m_children;
};
