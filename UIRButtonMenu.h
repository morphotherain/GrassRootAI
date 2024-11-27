#pragma once
#include "UIBase.h"
#include "UIButton.h"

class UIRButtonMenu : public UIBase
{
public:
	UIRButtonMenu() = default;
	UIRButtonMenu(UINT _source_id,UINT _target_id ) :
		source_object_id(_source_id), target_object_id(_target_id),m_button(){}
	~UIRButtonMenu() = default;

	// 虚基类Row，表示菜单项对应的按钮行相关信息
	struct Row {
		virtual ~Row() {}
		virtual void handleClick(std::shared_ptr<GameObject> sourceObject, std::shared_ptr<GameObject> targetObject) = 0;
		void InitButton(float x, float y, UINT count) {
			m_button = std::make_shared<UIButton>();
			m_button->setSize(x, y + count * 25.0f, 120.0f, 25.0f); count++;
			m_button->setTex("demoTex\\UI\\Window\\RbuttonMenu.dds");
			m_button->setText(text);
		}
		void setText(std::wstring _text) { text = _text; }
		std::wstring text;
		std::shared_ptr<UIButton> m_button;
	};

	// 示例的具体菜单项对应的派生类，以下以“详细信息”菜单项为例

	// 表示“详细信息”菜单项对应的行类
	class DetailInfoRow : public Row {
	public:
		DetailInfoRow() {
			setText(L"详细信息");
		}

		void handleClick(std::shared_ptr<GameObject> sourceObject, std::shared_ptr<GameObject> targetObject) override {
			// 实现“详细信息”菜单项的具体处理逻辑，比如弹出显示目标对象详细属性的窗口等
			// showObjectDetails(targetObject);
		}
	};

	// 表示“朝向该处”菜单项对应的行类
	class OrientRow : public Row {
	public:
		OrientRow() {
			setText(L"朝向该处");
		}

		void handleClick(std::shared_ptr<GameObject> sourceObject, std::shared_ptr<GameObject> targetObject) override {
			std::shared_ptr<Task> task = std::make_shared<Task>();
			task->isInnerTask = true;
			task->taskID = -1;
			task->publisher = sourceObject;
			task->target = targetObject;
			task->taskTypeId = 0;
			sourceObject->addTask(task);
			return;
		}
	};

	// 表示“环绕”菜单项对应的行类
	class OrbitRow : public Row {
	public:
		OrbitRow() {
			setText(L"环绕");
		}

		void handleClick(std::shared_ptr<GameObject> sourceObject, std::shared_ptr<GameObject> targetObject) override {
		}
	};
	// 表示“保持距离”菜单项对应的行类
	class MaintainDistanceRow : public Row {
	public:
		MaintainDistanceRow() {
			setText(L"保持距离");
		}

		void handleClick(std::shared_ptr<GameObject> sourceObject, std::shared_ptr<GameObject> targetObject) override {
		}
	};
	// 表示“保持距离”菜单项对应的行类
	class DockRow : public Row {
	public:
		DockRow() {
			setText(L"停靠");
		}

		void handleClick(std::shared_ptr<GameObject> sourceObject, std::shared_ptr<GameObject> targetObject) override {
			std::shared_ptr<Task> task = std::make_shared<Task>();
			task->isInnerTask = true;
			task->taskID = -1; 
			task->publisher = sourceObject;
			task->target = targetObject;
			task->taskTypeId = 0;
			targetObject->addTask(task);
			return;
		}
	};
	// 表示“跳跃”菜单项对应的行类
	class JumpRow : public Row {
	public:
		JumpRow() {
			setText(L"跳跃");
		}

		void handleClick(std::shared_ptr<GameObject> sourceObject, std::shared_ptr<GameObject> targetObject) override {
		}
	};
	// 表示“跳跃”菜单项对应的行类
	class WarpToRow : public Row {
	public:
		WarpToRow() {
			setText(L"跃迁到");
		}

		void handleClick(std::shared_ptr<GameObject> sourceObject, std::shared_ptr<GameObject> targetObject) override {
			std::shared_ptr<Task> task = std::make_shared<Task>();
			task->isInnerTask = true;
			task->taskID = -1;
			task->publisher = sourceObject;
			task->target = targetObject;
			task->taskTypeId = 1;
			sourceObject->addTask(task);
			return;
		}
	};

	std::vector<std::shared_ptr<Row>> m_Rows;

	virtual bool Init();
	virtual void OnResize() {};
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup() {};

	bool InitResource() {};
	bool InitEffect() {};
	void setSize(const float _x, const float _y) { x = _x, y = _y; }
	void addRow(std::shared_ptr<Row> row);

	std::vector<std::shared_ptr<UIButton>> m_button;
	UINT source_object_id;
	UINT target_object_id;
	UINT count = 0;

private:

	float x = 10.0f;
	float y = 30.0f;


};

