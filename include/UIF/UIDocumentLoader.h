#pragma once

#include "UIDocument.h"
#include "UIRect.h"
#include <memory>
#include <nlohmann/json.hpp>

class UIDocumentLoader
{
public:
	static std::shared_ptr<UIDocument> LoadFromFile(const std::string& path);
	static bool SaveToFile(const UIDocument& document, const std::string& path);

private:
	static std::shared_ptr<UINode> ParseNode(const nlohmann::json& json, int depth = 0);
	static nlohmann::json SerializeNode(const UINode& node);
	static UIRect ParseRect(const nlohmann::json& json);
};
