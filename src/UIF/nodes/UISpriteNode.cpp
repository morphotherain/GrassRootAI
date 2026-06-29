#include "UISpriteNode.h"
#include "UIAtlasRegistry.h"
#include "UIRenderContext.h"

bool UISpriteNode::ResolveSpriteSource(std::string& texturePath, float& u0, float& v0, float& u1, float& v1) const
{
	u0 = 0.0f;
	v0 = 0.0f;
	u1 = 1.0f;
	v1 = 1.0f;

	if (HasAtlasFrame())
	{
		UIAtlasResolveResult resolved;
		if (UIAtlasRegistry::Instance().ResolveFrame(m_atlasId, m_frameName, resolved))
		{
			texturePath = resolved.texturePath;
			u0 = resolved.u0;
			v0 = resolved.v0;
			u1 = resolved.u1;
			v1 = resolved.v1;
			return true;
		}
	}

	if (!m_texturePath.empty())
	{
		texturePath = m_texturePath;
		return true;
	}

	return false;
}

void UISpriteNode::Build(UIRenderContext& context)
{
	std::string texturePath;
	float u0 = 0.0f;
	float v0 = 0.0f;
	float u1 = 1.0f;
	float v1 = 1.0f;
	if (!ResolveSpriteSource(texturePath, u0, v0, u1, v1))
	{
		return;
	}

	context.GetOrCreateSpriteEffect(texturePath, m_rect.w, m_rect.h, u0, v0, u1, v1);
	m_built = true;
	UINode::Build(context);
}

void UISpriteNode::Draw(UIRenderContext& context, float absX, float absY)
{
	if (!m_visible)
	{
		return;
	}

	std::string texturePath;
	float u0 = 0.0f;
	float v0 = 0.0f;
	float u1 = 1.0f;
	float v1 = 1.0f;
	if (!ResolveSpriteSource(texturePath, u0, v0, u1, v1))
	{
		return;
	}

	const float drawX = absX + m_rect.x;
	const float drawY = absY + m_rect.y;
	context.DrawUIQuad(texturePath, drawX, drawY, m_rect.w, m_rect.h,
		GetScaleX(), GetScaleY(), GetRotation(), u0, v0, u1, v1);

	UINode::Draw(context, absX, absY);
}
