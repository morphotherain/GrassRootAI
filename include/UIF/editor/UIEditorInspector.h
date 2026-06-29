#pragma once

#include "UIRect.h"
#include "UINode.h"
#include <memory>
#include <string>
#include <vector>

class UIRenderContext;

enum class EditorFieldKind
{
	Text,
	Float,
	Bool
};

struct EditorTreeRowHit
{
	std::weak_ptr<UINode> node;
	UIRect bounds;
};

struct EditorFieldHit
{
	std::string id;
	EditorFieldKind kind = EditorFieldKind::Text;
	UIRect bounds;
};

struct EditorFieldSpec
{
	std::string id;
	std::string label;
	EditorFieldKind kind = EditorFieldKind::Text;
};

struct EditorPaletteHit
{
	std::string atlasId;
	std::string frameName;
	UIRect bounds;
};

class UIEditorInspector
{
public:
	void ClearFrameHits();

	void AddTreeRow(const std::shared_ptr<UINode>& node, const UIRect& bounds);
	std::shared_ptr<UINode> HitTestTree(float mouseX, float mouseY) const;

	void AddPaletteRow(const std::string& atlasId, const std::string& frameName, const UIRect& bounds) const;
	bool HitTestPalette(float mouseX, float mouseY, std::string& outAtlasId, std::string& outFrameName) const;
	float DrawFramePalette(UIRenderContext& context, const std::string& atlasId,
		float panelX, float panelTop, float panelW, float panelH) const;

	void BuildFieldSpecs(const UINode& node, std::vector<EditorFieldSpec>& outSpecs) const;
	float DrawPropertyPanel(UIRenderContext& context, const UINode& node,
		float panelX, float panelY, float panelW,
		const std::string& focusedFieldId, const std::string& editBuffer) const;

	std::string HitTestField(float mouseX, float mouseY, EditorFieldKind* outKind = nullptr) const;

	void BeginEdit(const UINode& node, const std::string& fieldId, std::string& outBuffer) const;
	bool CommitEdit(UINode& node, const std::string& fieldId, const std::string& buffer) const;
	bool ToggleBoolField(UINode& node, const std::string& fieldId) const;

	void AppendUtf8Char(std::string& buffer, wchar_t ch) const;
	void Backspace(std::string& buffer) const;
	std::string NextFieldId(const UINode& node, const std::string& currentFieldId) const;

	static std::string GetFieldDisplayValueUtf8(const UINode& node, const std::string& fieldId);

private:
	mutable std::vector<EditorTreeRowHit> m_treeRows;
	mutable std::vector<EditorFieldHit> m_fieldHits;
	mutable std::vector<EditorPaletteHit> m_paletteRows;
};
