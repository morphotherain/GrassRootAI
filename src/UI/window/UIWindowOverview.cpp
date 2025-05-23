﻿#include "UIWindowOverview.h"
#include "InvTypesManager.h"
#include "invGroupsManager.h"
#include <sstream>
#include <iomanip>
using namespace DirectX;

UIWindowOverview::UIWindowOverview() : UIWindow()
{
	UIWindow::UIWindow();
}

bool UIWindowOverview::Init()
{
	windowTitle = L"总览";

	InitWindowComponent();

	m_RowMgr = std::make_shared<RowMgr>(x, y, TitleHeight);
	m_RowMgr->index = 0;

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& component : childComponents) {
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}

	return false;
}

void UIWindowOverview::OnResize()
{
}

void UIWindowOverview::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	UIWindow::UpdateUI(dt, mouse, keyboard, tick);
	for (auto& component : childComponents) {
		component->setDelta(x, y);
	}

	if (m_RButtonMenu != nullptr)
		m_RButtonMenu->UpdateUI(dt, mouse, keyboard, tick);
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	*isCatchingMouse = false;
	if (mouseState.x > x && mouseState.x < (x + width) && mouseState.y > y && mouseState.y < (y + height)) {
		*isCatchingMouse = true;

		// 新增代码，处理鼠标滚轮滚动事件来改变index值
		m_RowMgr->next_index = m_RowMgr->index;
		int wheelDelta = mouseState.scrollWheelValue - lastMouseState.scrollWheelValue;
		DEBUG_("wheelDelta  : {},mouseState.scrollWheelValue  : {},lastMouseState.scrollWheelValue  : {}", wheelDelta, mouseState.scrollWheelValue, lastMouseState.scrollWheelValue);
		if (wheelDelta != 0) {
			if (wheelDelta > 0) {
				m_RowMgr->next_index--;  // 鼠标滚轮向上滚动，index减小，可根据实际需求调整增减幅度
			}
			else {
				m_RowMgr->next_index++;  // 鼠标滚轮向下滚动，index增大
			}
			// 可以在这里添加对index范围的限制，防止越界等情况，比如：
			if (m_RowMgr->next_index < 0) {
				m_RowMgr->next_index = 0;
			}
		}
		// 在鼠标没进入窗口前仍为ABSOLUTE模式
		if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
		{
			m_RButtonMenu = nullptr;
		}

		// 在鼠标没进入窗口前仍为ABSOLUTE模式
		if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.rightButton == true)
		{
			UINT currentID = SolarSystemMgr::getInstance().currentPilot->currentShip->GetComponent<BaseComponent>()->objectID;
			int index = static_cast<int>((mouseState.y - y - 35) / TitleHeight - 2);
			if (m_RowMgr->Rows.size() > index)
			{
				UINT targetID = m_RowMgr->Rows[index + m_RowMgr->next_index]->objectID;
				m_RButtonMenu = std::make_shared<UIRButtonMenu>(currentID, targetID);
				m_RButtonMenu->setSize(static_cast<float>(mouseState.x), static_cast<float>(mouseState.y));
				m_RButtonMenu->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
				m_RButtonMenu->Init();
			}
		}
	}
	m_RowMgr->setPos(x, y);
	m_RowMgr->Update(tick);
}

void UIWindowOverview::DrawUI()
{
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();

	XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);

	ConstantMVPIndex* dataPtr = m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(windowModel);
	dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_windowEffect->apply();
	for (auto& component : childComponents) {
		component->DrawUI();
	}
	m_RowMgr->Draw();
	if (m_RButtonMenu != nullptr)
		m_RButtonMenu->DrawUI();
}

void UIWindowOverview::cleanup()
{
}

bool UIWindowOverview::InitResource()
{
	return true;
}

bool UIWindowOverview::InitEffect()
{
	return true;
}

void UIWindowOverview::setSize(const float _x, const float _y, const float _deltaX, const float _deltaY)
{
	UIWindow::setSize(_x, _y, _deltaX, _deltaY);
}

void UIWindowOverview::InitWindowComponent()
{
	UIWindow::InitWindowComponent();
	auto Texture = m_windowEffect->getTextures();

	auto vertexs = m_windowEffect->getVertexBuffer<PosTexIndex>()->getVertices();
	GenerateRectVertex(vertices, 0.0f, TitleHeight, 50.0f, 30.0f, 7.0f);
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertices);
	m_windowEffect->Init();

	auto text = std::make_shared<UIText>();
	text->setSize(11.0f, TitleHeight + 5.0f, 350.0f, 350.0f);
	text->setText(L"默认");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(20.0f, TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"名称");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(230.0f, TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"距离");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(350.0f, TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"类型");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(460.0f, TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"速度");
	AddUIComponent(text);
}

void UIWindowOverview::Row::Init(float x, float y, float TitleHeight)
{
	std::shared_ptr<UIText> text = std::make_shared<UIText>();
	text->setSize(x + 20.0f, y + 2 * TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(Name);
	texts.push_back(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 240.0f, y + 2 * TitleHeight + 35.0f, 350.0f, 350.0f);
	std::wstring display_distance = getDisplay();
	text->setText(display_distance);
	texts.push_back(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 315.0f, y + 2 * TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(typeName);
	texts.push_back(text);

	std::wstringstream wss;
	wss << std::fixed << std::setprecision(1) << velocity << L"m/s";
	text = std::make_shared<UIText>();
	text->setSize(x + 490.0f, y + 2 * TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(wss.str());
	texts.push_back(text);

	for (auto& component : texts) {
		component->Init();
	}
}

void UIWindowOverview::Row::Update()
{
}

void UIWindowOverview::Row::Draw()
{
	for (auto& component : texts) {
		component->DrawUI();
	}
}

void UIWindowOverview::Row::setIndex(float x, float y, float TitleHeight)
{
	texts[0]->setSize(x + 10.0f, y + (2) * TitleHeight + 35.0f, 350.0f, 350.0f);
	texts[1]->setSize(x + 220.0f, y + (2) * TitleHeight + 35.0f, 350.0f, 350.0f);
	texts[2]->setSize(x + 300.0f, y + (2) * TitleHeight + 35.0f, 350.0f, 350.0f);
	texts[3]->setSize(x + 460.0f, y + (2) * TitleHeight + 35.0f, 350.0f, 350.0f);
}

std::wstring UIWindowOverview::Row::getDisplay()
{
	std::wstring display_distance;

	if (distance < 10000) {
		// 以米为单位显示，转换为整数
		std::wostringstream oss;
		oss << static_cast<int>(distance) << L" m";
		display_distance = oss.str();
	}
	else if (distance < 10000000) {
		// 以千米为单位显示，保留一位小数
		std::wostringstream oss;
		oss << std::fixed << std::setprecision(1) << distance / 1000 << L" km";
		display_distance = oss.str();
	}
	else {
		// 以天文单位（AU）为单位显示，按情况保留小数
		double distanceInAU = distance / AU_TO_METERS;
		std::wostringstream oss;
		if (distanceInAU < 0.01) {
			// 小于0.01AU时，直接输出 <0.01AU
			oss << L"<0.01AU";
		}
		else if (distanceInAU < 1) {
			// 如果小于1AU就保留两位小数
			oss << std::fixed << std::setprecision(2) << distanceInAU << L" AU";
		}
		else if (distanceInAU < 10) {
			// 如果小于10AU就保留一位小数
			oss << std::fixed << std::setprecision(1) << distanceInAU << L" AU";
		}
		else {
			oss << std::fixed << std::setprecision(0) << distanceInAU << L" AU";
		}
		display_distance = oss.str();
	}
	return display_distance;
}

void UIWindowOverview::RowMgr::Init()
{
}

void UIWindowOverview::RowMgr::Update(UINT tick)
{
	if (tick % 30 == 0) {
		Rows.clear();
		auto currentSolarSystem = SolarSystemMgr::getInstance().currentSolarSystem;
		auto currentSector = currentSolarSystem->currentSector;
		if (currentSector == nullptr)
			return;
		auto denormalizes = currentSolarSystem->m_denormalizes;
		auto spaceObjects = currentSector->space_objects;
		auto currentShip = SolarSystemMgr::getInstance().currentPilot->currentShip;
		auto currentTran = currentShip->GetComponent<SpaceTransformComponent>();
		auto currentBase = currentShip->GetComponent<BaseComponent>();
		int count = 0;
		int count_delta = 0;

		for (auto object : spaceObjects) {
			auto pobj = object.lock();
			std::shared_ptr<Row> temp = std::make_shared<Row>();
			auto base = pobj->GetComponent<BaseComponent>();
			auto Tran = pobj->GetComponent<SpaceTransformComponent>();
			auto Physics = pobj->GetComponent<PhysicsComponent>();

			if (base->objectID == currentBase->objectID)continue;

			temp->Name = base->name;
			temp->typeName = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
			temp->distance = currentTran->calculateDistance(*Tran);
			if (Physics != nullptr) {
				temp->velocity = Physics->CalculateSpeedMagnitude();
			}
			else
			{
				temp->velocity = 0.0f;
			}
			temp->objectID = base->objectID;
			temp->Init(x, y + count * Height, Height); count++;
			Rows.push_back(temp);
		}
		for (auto denormalize : denormalizes) {
			auto objectID = denormalize->itemID;
			auto object = (*SolarSystemMgr::getInstance().p_mapObject)[objectID];
			std::shared_ptr<Row> temp = std::make_shared<Row>();
			auto base = object->GetComponent<BaseComponent>();
			auto Tran = object->GetComponent<SpaceTransformComponent>();
			auto Physics = object->GetComponent<PhysicsComponent>();

			if (base->groupID == 7 || base->groupID == 8)continue;

			temp->Name = base->name;
			temp->typeName = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
			temp->distance = currentTran->calculateDistance(*Tran);
			if (temp->distance < 10000000)continue;
			if (Physics != nullptr) {
				temp->velocity = Physics->CalculateSpeedMagnitude();
			}
			else
			{
				temp->velocity = 0.0f;
			}
			temp->objectID = base->objectID;
			temp->Init(x, y + count * Height, Height); count++;
			Rows.push_back(temp);
		}
	}
}

void UIWindowOverview::RowMgr::Draw()
{
	index = next_index;
	int total = static_cast<int>(Rows.size());
	if (total <= 18)
		index = 0;
	else {
		if (index > total - 18)
			index = (total - 18);
	}

	int count = -1 * index;
	for (auto& component : Rows) {
		if (count < 0) {
			count++;
			continue;
		}
		component->setIndex(x, y + count * Height, Height);
		component->Draw();
		if (count++ == 18)break;
	}
}