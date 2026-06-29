#pragma once

#include "UIAtlas.h"
#include <string>
#include <vector>

class UIAtlasRegistry
{
public:
	static UIAtlasRegistry& Instance();

	bool LoadFromFile(const std::string& path);
	void LoadAtlasesFromDirectory(const std::string& directoryPath);
	void Clear();

	const UIAtlasDefinition* FindAtlas(const std::string& atlasId) const;
	const UIAtlasFrame* GetFrameDefinition(const std::string& atlasId, const std::string& frameName) const;
	std::vector<std::string> GetFrameNames(const std::string& atlasId) const;
	bool ResolveFrame(const std::string& atlasId, const std::string& frameName, UIAtlasResolveResult& out) const;

private:
	std::unordered_map<std::string, UIAtlasDefinition> m_atlases;
};
