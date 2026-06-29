#pragma once

#include "UINode.h"

class UIPanelNode : public UINode
{
public:
	void Draw(UIRenderContext& context, float absX, float absY) override;
};
