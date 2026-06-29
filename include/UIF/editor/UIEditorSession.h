#pragma once

#include "UIDocument.h"
#include "UIEditorInspector.h"
#include "UIInputState.h"
#include "UIRenderContext.h"
#include <Keyboard.h>
#include <memory>
#include <string>

class UISystem;

class UIEditorSession
{
public:
	void Attach(UISystem* system, const std::string& sourcePath);
	void SetActive(bool active) { m_active = active; }
	bool IsActive() const { return m_active; }

	void ToggleActive();

	void Update(UIInputState& input);
	void HandleEditorKeys(DirectX::Keyboard::KeyboardStateTracker& keyboardTracker,
		const DirectX::Keyboard::State& keyState);
	void OnChar(wchar_t ch);
	void Draw(UIRenderContext& context, float fps, float frameMs);

	bool SaveDocument();
	void AddChildNode(const std::string& nodeType);
	void AddAtlasNode(const std::string& atlasId, const std::string& frameName, bool asButton);
	bool DeleteSelectedNode();

	std::shared_ptr<UINode> GetSelectedNode() const { return m_selectedNode.lock(); }
	void SelectNode(const std::shared_ptr<UINode>& node);

	void Detach();

private:
	void MoveSelectedNode(DirectX::Keyboard::KeyboardStateTracker& keyboardTracker,
		const DirectX::Keyboard::State& keyState, bool resizeMode);
	void ToggleSelectedVisible();
	void HandlePanelClick(UIInputState& input);
	void FocusField(const std::shared_ptr<UINode>& node, const std::string& fieldId);
	void ClearFieldFocus();
	bool CommitFocusedField();
	void HandleFieldEditingKeys(DirectX::Keyboard::KeyboardStateTracker& keyboardTracker,
		const DirectX::Keyboard::State& keyState);

	UISystem* m_system = nullptr;
	std::string m_sourcePath;
	bool m_active = false;
	std::weak_ptr<UINode> m_selectedNode;
	int m_nodeCounter = 0;

	UIEditorInspector m_inspector;
	std::string m_focusedFieldId;
	std::string m_editBuffer;
};
