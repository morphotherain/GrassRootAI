#pragma once

#include "UINode.h"
#include <memory>
#include <string>

struct UIDocument
{
	int version = 1;
	std::string id;
	std::string sourcePath;
	std::shared_ptr<UINode> root;
};
