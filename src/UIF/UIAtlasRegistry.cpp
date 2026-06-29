#include "UIAtlasRegistry.h"
#include "logger_manager.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

UIAtlasRegistry& UIAtlasRegistry::Instance()
{
	static UIAtlasRegistry instance;
	return instance;
}

static UIAtlasFrame ParseAtlasFrame(const nlohmann::json& json, const UIAtlasDefinition& atlas)
{
	UIAtlasFrame frame;
	if (json.contains("texture"))
	{
		frame.texturePath = json["texture"].get<std::string>();
	}
	if (json.contains("x")) frame.x = json["x"].get<float>();
	if (json.contains("y")) frame.y = json["y"].get<float>();
	if (json.contains("w")) frame.w = json["w"].get<float>();
	if (json.contains("h")) frame.h = json["h"].get<float>();

	if (frame.texturePath.empty() && !atlas.texturePath.empty())
	{
		frame.texturePath = atlas.texturePath;
	}
	return frame;
}

bool UIAtlasRegistry::LoadFromFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		WARN_("UIAtlas 加载失败: {}", path);
		return false;
	}

	nlohmann::json json;
	file >> json;

	UIAtlasDefinition atlas;
	atlas.id = json.value("id", fs::path(path).stem().string());
	if (json.contains("texture"))
	{
		atlas.texturePath = json["texture"].get<std::string>();
	}
	if (json.contains("size"))
	{
		atlas.atlasWidth = json["size"].value("w", 0.0f);
		atlas.atlasHeight = json["size"].value("h", 0.0f);
	}

	if (!json.contains("frames") || !json["frames"].is_object())
	{
		WARN_("UIAtlas 缺少 frames: {}", path);
		return false;
	}

	for (const auto& [frameName, frameJson] : json["frames"].items())
	{
		atlas.frames[frameName] = ParseAtlasFrame(frameJson, atlas);
	}

	const std::string atlasId = atlas.id;
	const size_t frameCount = atlas.frames.size();
	m_atlases[atlasId] = std::move(atlas);
	INFO_("UIAtlas 已加载: {} ({} 帧) <- {}", atlasId, frameCount, path);
	return true;
}

void UIAtlasRegistry::LoadAtlasesFromDirectory(const std::string& directoryPath)
{
	if (!fs::exists(directoryPath))
	{
		WARN_("UIAtlas 目录不存在: {}", directoryPath);
		return;
	}

	for (const auto& entry : fs::directory_iterator(directoryPath))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}
		const auto ext = entry.path().extension().string();
		if (ext == ".json" || ext == ".atlas.json")
		{
			LoadFromFile(entry.path().string());
		}
	}
}

void UIAtlasRegistry::Clear()
{
	m_atlases.clear();
}

const UIAtlasDefinition* UIAtlasRegistry::FindAtlas(const std::string& atlasId) const
{
	const auto it = m_atlases.find(atlasId);
	return it != m_atlases.end() ? &it->second : nullptr;
}

const UIAtlasFrame* UIAtlasRegistry::GetFrameDefinition(const std::string& atlasId, const std::string& frameName) const
{
	const UIAtlasDefinition* atlas = FindAtlas(atlasId);
	if (!atlas)
	{
		return nullptr;
	}
	const auto it = atlas->frames.find(frameName);
	return it != atlas->frames.end() ? &it->second : nullptr;
}

std::vector<std::string> UIAtlasRegistry::GetFrameNames(const std::string& atlasId) const
{
	std::vector<std::string> names;
	const UIAtlasDefinition* atlas = FindAtlas(atlasId);
	if (!atlas)
	{
		return names;
	}
	names.reserve(atlas->frames.size());
	for (const auto& entry : atlas->frames)
	{
		names.push_back(entry.first);
	}
	std::sort(names.begin(), names.end());
	return names;
}

bool UIAtlasRegistry::ResolveFrame(const std::string& atlasId, const std::string& frameName, UIAtlasResolveResult& out) const
{
	out = {};
	const UIAtlasDefinition* atlas = FindAtlas(atlasId);
	if (!atlas)
	{
		return false;
	}

	const auto frameIt = atlas->frames.find(frameName);
	if (frameIt == atlas->frames.end())
	{
		return false;
	}

	const UIAtlasFrame& frame = frameIt->second;
	if (frame.texturePath.empty())
	{
		return false;
	}

	out.texturePath = frame.texturePath;
	out.frameWidth = frame.w;
	out.frameHeight = frame.h;

	const bool hasAtlasSize = atlas->atlasWidth > 0.0f && atlas->atlasHeight > 0.0f;
	const bool hasFrameRect = frame.w > 0.0f && frame.h > 0.0f;
	if (hasAtlasSize && hasFrameRect && frame.texturePath == atlas->texturePath)
	{
		out.u0 = frame.x / atlas->atlasWidth;
		out.v0 = frame.y / atlas->atlasHeight;
		out.u1 = (frame.x + frame.w) / atlas->atlasWidth;
		out.v1 = (frame.y + frame.h) / atlas->atlasHeight;
	}
	else
	{
		out.u0 = 0.0f;
		out.v0 = 0.0f;
		out.u1 = 1.0f;
		out.v1 = 1.0f;
	}

	out.valid = true;
	return true;
}
