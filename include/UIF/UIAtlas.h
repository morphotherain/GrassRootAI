#pragma once

#include <string>
#include <unordered_map>

struct UIAtlasFrame
{
	std::string texturePath;
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
};

struct UIAtlasDefinition
{
	std::string id;
	std::string texturePath;
	float atlasWidth = 0.0f;
	float atlasHeight = 0.0f;
	std::unordered_map<std::string, UIAtlasFrame> frames;
};

struct UIAtlasResolveResult
{
	std::string texturePath;
	float u0 = 0.0f;
	float v0 = 0.0f;
	float u1 = 1.0f;
	float v1 = 1.0f;
	float frameWidth = 0.0f;
	float frameHeight = 0.0f;
	bool valid = false;
};
