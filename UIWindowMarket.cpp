#include "UIWindowMarket.h"
#include "UIButton.h"
#include "UIText.h"

#include "InvTypesManager.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"

using namespace DirectX;

UIWindowMarket::UIWindowMarket() : UIWindow()
{
}

bool UIWindowMarket::Init()
{
	windowTitle = L"市场";

	InitWindowComponent();

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

void UIWindowMarket::OnResize()
{
}

void UIWindowMarket::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	UIWindow::UpdateUI(dt, mouse, keyboard, tick);

	for (auto& component : childComponents) {
		component->setDelta(x, y);
	}
	for (auto& categoryText : categoryTexts)
	{
		categoryText.second->setDelta(x, y);
	}
	for (auto& groupText : groupTexts)
	{
		groupText.second->setDelta(x, y);
	}
	for (auto& typeText : typeTexts)
	{
		typeText.second->setDelta(x, y);
	}
}

void UIWindowMarket::DrawUI()
{
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();
	XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);
	ConstantMVPIndex* dataPtr = m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(windowModel);
	dataPtr->view = XMMatrixTranspose(viewMatrix);
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_windowEffect->apply();

	for (auto& component : childComponents) {
		component->DrawUI();
	}
	currentY = y; // 每次绘制时重置纵坐标

	for (auto& categoryPair : categoryTexts)
	{
		int categoryID = categoryPair.first;
		auto categoryText = categoryPair.second;
		categoryText->setDelta(x, currentY);
		categoryText->DrawUI();

		if (categoryExpanded[categoryID])
		{
			for (int groupID : categoryGroups[categoryID])
			{
				auto groupText = groupTexts[groupID];
				currentY += 20; // 每次绘制后增加纵坐标
				groupText->setDelta(x + 20, currentY);
				groupText->DrawUI();

				if (groupExpanded[groupID])
				{
					for (int typeID : groupTypes[groupID])
					{
						auto typeText = typeTexts[typeID];
						currentY += 20; // 每次绘制后增加纵坐标
						typeText->setDelta(x + 40, currentY);
						typeText->DrawUI();
					}
				}
			}
		}

		currentY += 20; // 每个category绘制完后增加纵坐标
		if (currentY - y > 400)break;
	}
}

void UIWindowMarket::cleanup()
{
}

bool UIWindowMarket::InitResource()
{
	return true;
}

bool UIWindowMarket::InitEffect()
{
	return true;
}


void UIWindowMarket::ParseParameters(std::unordered_map<std::string, std::any> paras)
{
	x = 300;
	y = 400;
	deltaX = 800;
	deltaY = 600;
	UIWindow::setSize(x, y, deltaX, deltaY);
}

void UIWindowMarket::InitWindowComponent()
{
	UIWindow::InitWindowComponent();

	m_windowEffect->Init();

	std::vector<int> categoryIDs = invCategoriesManager::getInstance()->getAllCategoriesID();

	for (int categoryID : categoryIDs)
	{
		auto categoryText = std::make_shared<UIText>();
		categoryText->setSize(10, 30, 200, 20); // 初始大小设置，坐标后续在DrawUI中更新
		categoryText->setText(invCategoriesManager::getInstance()->getNameByCategoriesID(categoryID));
		categoryText->Init();
		categoryTexts[categoryID] = categoryText;

		std::vector<int> groupIDs = invGroupsManager::getInstance()->getAllGroupIDByCategoriesID(categoryID);
		categoryGroups[categoryID] = groupIDs;
		categoryExpanded[categoryID] = false;
	}

	for (auto& categoryPair : categoryTexts)
	{
		int categoryID = categoryPair.first;
		for (int groupID : categoryGroups[categoryID])
		{
			auto groupText = std::make_shared<UIText>();
			groupText->setSize(15, 30, 200, 20); // 初始大小设置，坐标后续在DrawUI中更新
			groupText->setText(invGroupsManager::getInstance()->getNameByGroupId(groupID));
			groupText->Init();
			groupTexts[groupID] = groupText;

			std::vector<int> typeIDs = InvTypesManager::getInstance()->getAllTypeIDByGroupID(groupID);
			groupTypes[groupID] = typeIDs;
			groupExpanded[groupID] = false;
		}
	}

	for (auto& groupPair : groupTexts)
	{
		int groupID = groupPair.first;
		for (int typeID : groupTypes[groupID])
		{
			auto typeText = std::make_shared<UIText>();
			typeText->setSize(20, 30, 200, 20); // 初始大小设置，坐标后续在DrawUI中更新
			typeText->setText(InvTypesManager::getInstance()->getNameByTypeId(typeID));
			typeText->Init();
			typeTexts[typeID] = typeText;
		}
	}
}

void UIWindowMarket::ExpandCategory(int categoryID)
{
	categoryExpanded[categoryID] = true;
	for (int groupID : categoryGroups[categoryID])
	{
		groupExpanded[groupID] = true;
	}
}

void UIWindowMarket::ExpandGroup(int groupID)
{
	groupExpanded[groupID] = true;
}

void UIWindowMarket::CollapseCategory(int categoryID)
{
	categoryExpanded[categoryID] = false;
	for (int groupID : categoryGroups[categoryID])
	{
		groupExpanded[groupID] = false;
	}
}

void UIWindowMarket::CollapseGroup(int groupID)
{
	groupExpanded[groupID] = false;
}
