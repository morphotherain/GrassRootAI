#pragma once

#include "UINode.h"
#include <string>

class UIRenderContext;

class UISpriteNode : public UINode
{
public:
	const std::string& GetTexturePath() const { return m_texturePath; }
	void SetTexturePath(const std::string& path) { m_texturePath = path; m_built = false; }

	const std::string& GetAtlasId() const { return m_atlasId; }
	void SetAtlasId(const std::string& atlasId) { m_atlasId = atlasId; m_built = false; }

	const std::string& GetFrameName() const { return m_frameName; }
	void SetFrameName(const std::string& frameName) { m_frameName = frameName; m_built = false; }

	bool HasAtlasFrame() const { return !m_atlasId.empty() && !m_frameName.empty(); }

	void Build(UIRenderContext& context) override;
	void Draw(UIRenderContext& context, float absX, float absY) override;

protected:
	bool ResolveSpriteSource(std::string& texturePath, float& u0, float& v0, float& u1, float& v1) const;

	std::string m_texturePath;
	std::string m_atlasId;
	std::string m_frameName;
	bool m_built = false;
};
