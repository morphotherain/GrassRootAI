#include "UIRButtonMenu.h"

bool UIRButtonMenu::Init()
{

	auto source_object = (*SolarSystemMgr::getInstance().p_mapObject)[source_object_id];
	auto target_object = (*SolarSystemMgr::getInstance().p_mapObject)[target_object_id];
	if (source_object == nullptr || target_object == nullptr)return false;
	UINT count = 0;
	auto Base = target_object->GetComponent<BaseComponent>();
	if (Base != nullptr) {
		addRow(count++, L"详细信息");
	}

	auto Tran = target_object->GetComponent<SpaceTransformComponent>();
	if (Tran != nullptr)
	{
		addRow(count++, L"朝向该处");
		addRow(count++, L"环绕(10km)");
		addRow(count++, L"保持距离(10km)");
	}

	for (auto& component : m_button) {
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}

	return true;
}

void UIRButtonMenu::DrawUI()
{
	for (auto button : m_button) {
		button->DrawUI();
	}
}

void UIRButtonMenu::addRow(UINT count, std::wstring text)
{
	auto temp = std::make_shared<UIButton>();
	temp->setSize(x, y + count * 25.0f, 120.0f, 25.0f); count++;
	temp->setTex("demoTex\\UI\\Window\\RbuttonMenu.dds");
	temp->setText(text);
	m_button.push_back(temp);
}
