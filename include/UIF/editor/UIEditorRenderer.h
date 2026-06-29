#pragma once

#include "UIRenderContext.h"
#include "UINode.h"
#include <string>

class UIEditorInspector;

class UIEditorRenderer
{
public:
	void DrawDimOverlay(UIRenderContext& context);
	void DrawToolbar(UIRenderContext& context, float fps, float frameMs);
	void DrawPanelBackground(UIRenderContext& context, float x, float y, float w, float h);

	void DrawNodeOutline(UIRenderContext& context, const UINode& node, float absX, float absY, bool selected);
	void DrawNodeLabel(UIRenderContext& context, const UINode& node, float absX, float absY, bool selected);

	void DrawTreePanel(UIRenderContext& context, UIEditorInspector& inspector,
		const UINode& node, float panelX, float& panelY, int depth, const UINode* selected);

	static std::string GetNodeTypeName(const UINode& node);
};
