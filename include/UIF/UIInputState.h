#pragma once

#include <memory>

class UINode;

struct UIInputState
{
	float mouseX = 0.0f;
	float mouseY = 0.0f;
	bool mouseLeftDown = false;
	bool mouseLeftPressed = false;
	bool mouseLeftReleased = false;

	std::weak_ptr<UINode> hoverNode;
	std::weak_ptr<UINode> captureNode;
};
