#include "UIDocumentLoader.h"
#include "UIPanelNode.h"
#include "UITextNode.h"
#include "UISpriteNode.h"
#include "UIButtonNode.h"
#include <fstream>
#include <Windows.h>

namespace
{
	std::string WideToUtf8(const std::wstring& text)
	{
		if (text.empty())
		{
			return {};
		}
		const int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
		if (size <= 0)
		{
			return {};
		}
		std::string utf8(static_cast<size_t>(size), '\0');
		WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), utf8.data(), size, nullptr, nullptr);
		return utf8;
	}

	void ApplySpriteSourceFields(UISpriteNode& sprite, const nlohmann::json& json)
	{
		if (json.contains("atlas"))
		{
			sprite.SetAtlasId(json["atlas"].get<std::string>());
		}
		if (json.contains("frame"))
		{
			sprite.SetFrameName(json["frame"].get<std::string>());
		}
		if (json.contains("texture"))
		{
			sprite.SetTexturePath(json["texture"].get<std::string>());
		}
	}

	void SerializeSpriteSourceFields(const UISpriteNode& sprite, nlohmann::json& json)
	{
		if (sprite.HasAtlasFrame())
		{
			json["atlas"] = sprite.GetAtlasId();
			json["frame"] = sprite.GetFrameName();
		}
		if (!sprite.GetTexturePath().empty())
		{
			json["texture"] = sprite.GetTexturePath();
		}
	}
}

UIRect UIDocumentLoader::ParseRect(const nlohmann::json& json)
{
	UIRect rect;
	if (json.contains("x")) rect.x = json["x"].get<float>();
	if (json.contains("y")) rect.y = json["y"].get<float>();
	if (json.contains("w")) rect.w = json["w"].get<float>();
	if (json.contains("h")) rect.h = json["h"].get<float>();
	return rect;
}

std::shared_ptr<UINode> UIDocumentLoader::ParseNode(const nlohmann::json& json, int depth)
{
	if (depth > 64)
	{
		return nullptr;
	}
	if (!json.contains("type"))
	{
		return nullptr;
	}

	const std::string type = json["type"].get<std::string>();
	std::shared_ptr<UINode> node;

	if (type == "Panel")
	{
		node = std::make_shared<UIPanelNode>();
	}
	else if (type == "Text")
	{
		auto textNode = std::make_shared<UITextNode>();
		if (json.contains("text"))
		{
			textNode->SetTextUtf8(json["text"].get<std::string>());
		}
		node = textNode;
	}
	else if (type == "Sprite")
	{
		auto spriteNode = std::make_shared<UISpriteNode>();
		ApplySpriteSourceFields(*spriteNode, json);
		node = spriteNode;
	}
	else if (type == "Button")
	{
		auto buttonNode = std::make_shared<UIButtonNode>();
		ApplySpriteSourceFields(*buttonNode, json);
		if (!buttonNode->HasAtlasFrame() && !json.contains("texture"))
		{
			buttonNode->SetAtlasId("ui_window");
			buttonNode->SetFrameName("window_line");
		}
		if (json.contains("text"))
		{
			buttonNode->SetLabelUtf8(json["text"].get<std::string>());
		}
		if (json.contains("action"))
		{
			buttonNode->SetActionId(json["action"].get<std::string>());
		}
		node = buttonNode;
	}
	else
	{
		return nullptr;
	}

	if (json.contains("id"))
	{
		node->SetId(json["id"].get<std::string>());
	}
	if (json.contains("rect"))
	{
		node->SetRect(ParseRect(json["rect"]));
	}
	if (json.contains("visible"))
	{
		node->SetVisible(json["visible"].get<bool>());
	}
	if (json.contains("scale"))
	{
		if (json["scale"].is_number())
		{
			const float scale = json["scale"].get<float>();
			node->SetScaleX(scale);
			node->SetScaleY(scale);
		}
		else if (json["scale"].is_object())
		{
			if (json["scale"].contains("x")) node->SetScaleX(json["scale"]["x"].get<float>());
			if (json["scale"].contains("y")) node->SetScaleY(json["scale"]["y"].get<float>());
		}
	}
	if (json.contains("rotation"))
	{
		node->SetRotation(json["rotation"].get<float>());
	}

	if (type == "Text")
	{
		auto textNode = std::static_pointer_cast<UITextNode>(node);
		if (json.contains("fontSize"))
		{
			textNode->SetFontSize(json["fontSize"].get<float>());
		}
	}

	if (json.contains("children") && json["children"].is_array())
	{
		const auto& children = json["children"];
		if (children.size() > 4096)
		{
			return node;
		}
		for (const auto& childJson : children)
		{
			if (auto child = ParseNode(childJson, depth + 1))
			{
				node->AddChild(child);
			}
		}
	}

	return node;
}

nlohmann::json UIDocumentLoader::SerializeNode(const UINode& node)
{
	nlohmann::json json;
	json["id"] = node.GetId();
	json["rect"] = {
		{"x", node.GetRect().x},
		{"y", node.GetRect().y},
		{"w", node.GetRect().w},
		{"h", node.GetRect().h}
	};
	json["visible"] = node.IsVisible();
	if (node.GetScaleX() != 1.0f || node.GetScaleY() != 1.0f)
	{
		if (node.GetScaleX() == node.GetScaleY())
		{
			json["scale"] = node.GetScaleX();
		}
		else
		{
			json["scale"] = { {"x", node.GetScaleX()}, {"y", node.GetScaleY()} };
		}
	}
	if (node.GetRotation() != 0.0f)
	{
		json["rotation"] = node.GetRotation();
	}

	if (dynamic_cast<const UIPanelNode*>(&node))
	{
		json["type"] = "Panel";
	}
	else if (const auto* textNode = dynamic_cast<const UITextNode*>(&node))
	{
		json["type"] = "Text";
		if (textNode->GetFontSize() != 24.0f)
		{
			json["fontSize"] = textNode->GetFontSize();
		}
		// wide to utf8 simplified - store as empty if conversion fails
		const auto& w = textNode->GetText();
		if (!w.empty())
		{
			json["text"] = WideToUtf8(w);
		}
	}
	else if (const auto* buttonNode = dynamic_cast<const UIButtonNode*>(&node))
	{
		json["type"] = "Button";
		SerializeSpriteSourceFields(*buttonNode, json);
		json["action"] = buttonNode->GetActionId();
		const auto& w = buttonNode->GetLabel();
		if (!w.empty())
		{
			json["text"] = WideToUtf8(w);
		}
	}
	else if (const auto* spriteNode = dynamic_cast<const UISpriteNode*>(&node))
	{
		json["type"] = "Sprite";
		SerializeSpriteSourceFields(*spriteNode, json);
	}
	else
	{
		json["type"] = "Panel";
	}

	if (!node.GetChildren().empty())
	{
		nlohmann::json children = nlohmann::json::array();
		for (const auto& child : node.GetChildren())
		{
			children.push_back(SerializeNode(*child));
		}
		json["children"] = children;
	}

	return json;
}

std::shared_ptr<UIDocument> UIDocumentLoader::LoadFromFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return nullptr;
	}

	nlohmann::json json;
	file >> json;

	auto document = std::make_shared<UIDocument>();
	document->sourcePath = path;
	document->version = json.value("version", 1);
	document->id = json.value("id", std::string{});

	if (!json.contains("root"))
	{
		return nullptr;
	}

	document->root = ParseNode(json["root"]);
	if (!document->root)
	{
		return nullptr;
	}

	return document;
}

bool UIDocumentLoader::SaveToFile(const UIDocument& document, const std::string& path)
{
	if (!document.root)
	{
		return false;
	}

	nlohmann::json json;
	json["version"] = document.version;
	json["id"] = document.id;
	json["root"] = SerializeNode(*document.root);

	std::ofstream file(path);
	if (!file.is_open())
	{
		return false;
	}
	file << json.dump(2);
	return true;
}
