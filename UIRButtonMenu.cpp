#include "UIRButtonMenu.h"
using namespace DirectX; 

bool UIRButtonMenu::Init()
{
	UINT count = 0;
	double distance = 0.0f;
	auto source_object = (*SolarSystemMgr::getInstance().p_mapObject)[source_object_id];
	auto target_object = (*SolarSystemMgr::getInstance().p_mapObject)[target_object_id];
	if (source_object == nullptr || target_object == nullptr)return false;
	
	auto Base = target_object->GetComponent<BaseComponent>();
	if (Base != nullptr) {
		addRow(std::make_shared<DetailInfoRow>());
	}

	auto Tran = target_object->GetComponent<SpaceTransformComponent>();
	if (Tran != nullptr)
	{
		auto sourceTran = source_object->GetComponent<SpaceTransformComponent>();
		addRow(std::make_shared<OrientRow>());
		distance = sourceTran->calculateDistance(*Tran);
		if (distance >= 150000) {
			addRow(std::make_shared<WarpToRow>());
		}
		if (distance < 10000000) {
			addRow(std::make_shared<OrbitRow>());
			addRow(std::make_shared<MaintainDistanceRow>());
		}
	}
	if(distance < 2500)
	{
		auto Station = target_object->GetComponent<StationComponent>();
		if (Station != nullptr)
		{
			addRow(std::make_shared<DockRow>());
		}
	}

	auto Warp = target_object->GetComponent<WarpGateComponent>();
	if (Warp != nullptr)
	{
		addRow(std::make_shared<JumpRow>());
	}

	return true;
}

void UIRButtonMenu::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	if (mouseState.x > x && mouseState.x < (x + 125)) {
		// 在鼠标没进入窗口前仍为ABSOLUTE模式
		if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
		{
			UINT currentID = SolarSystemMgr::getInstance().currentPilot->currentShip->GetComponent<BaseComponent>()->objectID;
			int index = static_cast<int>((mouseState.y - y) / 25.0f);
			if (index >= 0 && m_Rows.size() > index)
			{
				auto source_object = (*SolarSystemMgr::getInstance().p_mapObject)[source_object_id];
				auto target_object = (*SolarSystemMgr::getInstance().p_mapObject)[target_object_id];
				m_Rows[index]->handleClick(source_object, target_object);
			}
		}
	}
}

void UIRButtonMenu::DrawUI()
{
	for (auto row : m_Rows) {
		row->m_button->DrawUI();
	}
}

void UIRButtonMenu::addRow(std::shared_ptr<Row> row)
{
	row->InitButton(x, y, count); count++;
	row->m_button->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
	row->m_button->Init();
	m_Rows.push_back(row);
}
