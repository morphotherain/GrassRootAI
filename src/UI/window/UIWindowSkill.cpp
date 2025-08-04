#include "UIWindowSkill.h"
#include "UIButton.h"
#include "UIText.h"

#include "InvTypesManager.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"

using namespace DirectX;

UIWindowSkill::UIWindowSkill() : UIWindow()
{
}

bool UIWindowSkill::Init()
{
	windowTitle = L"技能列表";

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

void UIWindowSkill::OnResize()
{
}

void UIWindowSkill::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	if (tick % 70 == 0)
		switchSelectType(selectTypeId);

	UIWindow::UpdateUI(dt, mouse, keyboard, tick);
	UpdateUIInfo(dt, mouse, keyboard, tick);
	UpdateSkillQueue(dt, mouse, keyboard, tick);


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

	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);


	// 检测鼠标左键是否刚被按下（上升沿检测）
	bool leftButtonPressed = mouseState.leftButton && !lastMouseState.leftButton;
	// 在鼠标没进入窗口前仍为ABSOLUTE
	if (x < mouseState.x && (x + 200) > mouseState.x && (y + 600) > mouseState.y && y < mouseState.y)
	{
		if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && leftButtonPressed)
		{
			lastIndexClickY = mouseState.y;
		}
		int wheelDelta = mouseState.scrollWheelValue - lastMouseState.scrollWheelValue;
		if (wheelDelta != 0) {
			if (wheelDelta > 0) {
				offsetY += 10;
			}
			else {
				offsetY -= 10;
			}
		}
	}
	
}

void UIWindowSkill::DrawUI()
{
	DrawIndex();
	DrawInfo();
	DrawSkillQueue();
}

void UIWindowSkill::cleanup()
{
}

void UIWindowSkill::DrawIndex()
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

	// 1. 保存当前视口
	UINT numViewports = 1;
	D3D11_VIEWPORT originalViewport;
	auto m_pd3dImmediateContext = D3DManager::getInstance().getDeviceContext();
	m_pd3dImmediateContext->RSGetViewports(&numViewports, &originalViewport);

	// 2. 设置新的视口
	D3D11_VIEWPORT adjustedViewport = originalViewport;
	adjustedViewport.TopLeftX = x;
	adjustedViewport.TopLeftY = y + 50.0f;
	adjustedViewport.Width = width;
	adjustedViewport.Height = height - 50.0f;
	m_pd3dImmediateContext->RSSetViewports(1, &adjustedViewport);


	// 3. 根据视口变化计算调整因子
	float viewportWidthRatio = originalViewport.Width / adjustedViewport.Width;
	float viewportHeightRatio = originalViewport.Height / adjustedViewport.Height;

	// 4. 创建一个缩放矩阵来抵消视口缩放影响
	XMMATRIX scaleMatrix = XMMatrixScaling(viewportWidthRatio, viewportHeightRatio, 1.0f);

	// 5. 调整投影矩阵
	XMMATRIX adjustedProjectionMatrix = XMMatrixMultiply(projMatrix, scaleMatrix);

	float adjustedX = 568.0f;
	float adjustedY = 234.0f;


	for (auto& component : childComponents) {
		component->DrawUI();
	}
	currentY = y + 50 + offsetY; // 每次绘制时重置纵坐标

	for (auto& categoryPair : categoryTexts)
	{
		int categoryID = categoryPair.first;
		auto categoryText = categoryPair.second;
		categoryText->setDelta(x + 40, currentY);
		categoryText->setClipRect({ x,y + 50,x + deltaX,y + deltaY });
		categoryText->DrawUI();

		auto arrowEffect = categoryArrows[categoryID];
		if (categoryExpanded[categoryID])
			arrowEffect = categoryArrowsExpanded[categoryID];
		XMMATRIX windowModelArrow = XMMatrixTranslation(adjustedX, currentY - y + adjustedY, 0.0f);
		ConstantMVPIndex* dataPtr = arrowEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtr->model = XMMatrixTranspose(windowModelArrow);
		dataPtr->view = XMMatrixTranspose(viewMatrix);
		dataPtr->projection = XMMatrixTranspose(adjustedProjectionMatrix);
		dataPtr->TexIndex = 0;
		arrowEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		arrowEffect->apply();

		ConstantMVPIndex* dataPtrBgr = categoryBgrEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtrBgr->model = XMMatrixTranspose(windowModelArrow);
		dataPtrBgr->view = XMMatrixTranspose(viewMatrix);
		dataPtrBgr->projection = XMMatrixTranspose(adjustedProjectionMatrix);
		dataPtrBgr->TexIndex = 0;
		categoryBgrEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		categoryBgrEffect->apply();

		ConstantMVPIndex* dataPtrIcon = categoryIcons[categoryID]->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtrIcon->model = XMMatrixTranspose(windowModelArrow);
		dataPtrIcon->view = XMMatrixTranspose(viewMatrix);
		dataPtrIcon->projection = XMMatrixTranspose(adjustedProjectionMatrix);
		dataPtrIcon->TexIndex = 0;
		categoryIcons[categoryID]->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		categoryIcons[categoryID]->apply();

		if (lastIndexClickY > currentY && lastIndexClickY < currentY + 20)
		{
			if (!categoryExpanded[categoryID])
				ExpandCategory(categoryID);
			else
				CollapseCategory(categoryID);
			lastIndexClickY = -10000;
		}


		if (categoryExpanded[categoryID])
		{
			for (int groupID : categoryGroups[categoryID])
			{
				auto groupText = groupTexts[groupID];
				currentY += 20; // 每次绘制后增加纵坐标
				groupText->setDelta(x + 60, currentY);
				groupText->setClipRect({ x,y + 50,x + deltaX,y + deltaY });
				groupText->DrawUI();

				auto arrowGroupEffect = groupArrows[groupID];
				if (groupExpanded[groupID])
					arrowGroupEffect = groupArrowsExpanded[groupID];
				XMMATRIX windowModelGroupArrow = XMMatrixTranslation(adjustedX, currentY - y + adjustedY, 0.0f);
				ConstantMVPIndex* dataPtr = arrowGroupEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
				dataPtr->model = XMMatrixTranspose(windowModelGroupArrow);
				dataPtr->view = XMMatrixTranspose(viewMatrix);
				dataPtr->projection = XMMatrixTranspose(adjustedProjectionMatrix);
				dataPtr->TexIndex = 0;
				arrowGroupEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
				arrowGroupEffect->apply();

				ConstantMVPIndex* dataPtrIcon = groupIcons[groupID]->getConstantBuffer<ConstantMVPIndex>()->Map();
				dataPtrIcon->model = XMMatrixTranspose(windowModelGroupArrow);
				dataPtrIcon->view = XMMatrixTranspose(viewMatrix);
				dataPtrIcon->projection = XMMatrixTranspose(adjustedProjectionMatrix);
				dataPtrIcon->TexIndex = 0;
				groupIcons[groupID]->getConstantBuffer<ConstantMVPIndex>()->Unmap();
				groupIcons[groupID]->apply();


				if (lastIndexClickY > currentY && lastIndexClickY < currentY + 20)
				{
					if (!groupExpanded[groupID])
						ExpandGroup(groupID);
					else
						CollapseGroup(groupID);
					lastIndexClickY = -10000;
				}

				if (groupExpanded[groupID])
				{
					for (int typeID : groupTypes[groupID])
					{
						auto typeText = typeTexts[typeID];
						currentY += 20; // 每次绘制后增加纵坐标
						typeText->setDelta(x + 60, currentY);
						typeText->setClipRect({ x,y + 50 ,x + 200,y + deltaY });
						typeText->DrawUI();
						if (lastIndexClickY > currentY && lastIndexClickY < currentY + 20)
						{
							switchSelectType(typeID);
							lastIndexClickY = -10000;
						}

						if (selectTypeId == typeID) {
							XMMATRIX windowModelSelectTypeBgr = XMMatrixTranslation(adjustedX, currentY - y + adjustedY, 0.0f);
							ConstantMVPIndex* dataPtrBgr = categoryBgrEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
							dataPtrBgr->model = XMMatrixTranspose(windowModelSelectTypeBgr);
							dataPtrBgr->view = XMMatrixTranspose(viewMatrix);
							dataPtrBgr->projection = XMMatrixTranspose(adjustedProjectionMatrix);
							dataPtrBgr->TexIndex = 0;
							categoryBgrEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
							categoryBgrEffect->apply();
						}
					}
				}
			}
		}

		currentY += 20; // 每个category绘制完后增加纵坐标

		if (currentY - y > 600)break;
	}

	m_pd3dImmediateContext->RSSetViewports(1, &originalViewport);
}

void UIWindowSkill::DrawInfo()
{
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();
	XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);
	ConstantMVPIndex* dataPtr = selectTypeImgEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(windowModel);
	dataPtr->view = XMMatrixTranspose(viewMatrix);
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	selectTypeImgEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	selectTypeImgEffect->apply();

	selectTypeNameText->setDelta(x, y);
	selectTypeNameText->DrawUI();
	skillQueueTileText->setDelta(x, y);
	skillQueueTileText->DrawUI();
	selectTypeGroupText->setDelta(x, y);
	selectTypeGroupText->DrawUI();

	{
		ConstantMVPIndex* dataPtr = m_skillIndicator->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtr->model = XMMatrixTranspose(windowModel);
		dataPtr->view = XMMatrixTranspose(viewMatrix);
		dataPtr->projection = XMMatrixTranspose(projMatrix);
		dataPtr->TexIndex = 0;
		m_skillIndicator->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		m_skillIndicator->apply();
	}

	skillAddButton->DrawUI();
}

void UIWindowSkill::UpdateUIInfo(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	skillAddButton->setDelta(x, y);
	skillAddButton->UpdateUI(dt, mouse, keyboard, tick);
	if (*(skillAddButton->getClickFlag())) {
		*(skillAddButton->getClickFlag()) = false;
		auto task = std::make_shared<Task>();
		auto pilot = SolarSystemMgr::getInstance().currentPilot;
		task->publisher = pilot;
		task->target = pilot;
		(*task->paramsPtr)["taskType"] = std::string("skillComponent");
		(*task->paramsPtr)["skillTaskType"] = std::string("AddToActiveQueue");
		(*task->paramsPtr)["skillTypeId"] = static_cast<int>(selectTypeId);
		TaskMgr::getInstance().addTask(task);
		//
	}

}

bool UIWindowSkill::InitResource()
{
	return true;
}

bool UIWindowSkill::InitEffect()
{
	return true;
}


void UIWindowSkill::ParseParameters(std::unordered_map<std::string, std::any> paras)
{
	x = 300;
	y = 400;
	deltaX = 800;
	deltaY = 600;
	UIWindow::setSize(x, y, deltaX, deltaY);
}

void UIWindowSkill::InitWindowComponent()
{
	UIWindow::InitWindowComponent();

	auto vertexs = m_windowEffect->getVertexBuffer<PosTexIndex>()->getVertices();
	GenerateRectVertex(vertexs, 0.0f, 45.0f, 230.0f, 1.0f, 7.0f);
	GenerateRectVertex(vertexs, 250.0f, 45.0f, deltaX - 250.0f, 1.0f, 7.0f);
	GenerateRectVertex(vertexs, 250.0f, 140.0f, deltaX - 250.0f, 1.0f, 7.0f);
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexs);
	m_windowEffect->addBlendState();
	m_windowEffect->Init();

	std::vector<int> categoryIDs = { 16 };

	for (int categoryID : categoryIDs)
	{
		auto typeID = InvTypesManager::getInstance()->getMinTypeIdByCategoryIDIcon(categoryID);
		auto IconPath = getIconPathByTypeID(typeID);
		auto m_itemImgEffect = std::make_shared<Effect>();
		m_itemImgEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		m_itemImgEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
		m_itemImgEffect->addTextures({ IconPath });
		std::vector<PosTexIndex> vertexsImg = {};
		GenerateRectVertex(vertexsImg, 28.0f, 0.0f, 20.0f, 20.0f, 0.0f);
		m_itemImgEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsImg);
		m_itemImgEffect->addConstantBuffer<ConstantMVPIndex>();
		m_itemImgEffect->Init();
		categoryIcons[categoryID] = m_itemImgEffect;


		auto categoryText = std::make_shared<UIText>();
		categoryText->setSize(10, 0, 200, 20); // 初始大小设置，坐标后续在DrawUI中更新
		categoryText->setText(invCategoriesManager::getInstance()->getNameByCategoriesID(categoryID));
		categoryText->switchTextFormat("Bold_XS");
		categoryText->Init();
		categoryTexts[categoryID] = categoryText;

		auto m_arrowEffect = std::make_shared<Effect>();
		m_arrowEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		m_arrowEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
		m_arrowEffect->addTextures({ "demoTex\\EVE\\media\\res\\Uprising_V21.03_Icons\\Icons\\UI\\ShipUI\\dds\\optionsBtn.dds" });

		std::vector<PosTexIndex> vertexsBack = {};
		GenerateRectVertex(vertexsBack, 15.0f, 2.0f, 12.0f, 12.0f, 0.0f);
		m_arrowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsBack);
		m_arrowEffect->addConstantBuffer<ConstantMVPIndex>();
		m_arrowEffect->Init();
		categoryArrows[categoryID] = m_arrowEffect;

		auto m_arrowExpandEffect = std::make_shared<Effect>();
		m_arrowExpandEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		m_arrowExpandEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
		m_arrowExpandEffect->addTextures({ "demoTex\\EVE\\media\\res\\Uprising_V21.03_Icons\\Icons\\UI\\ShipUI\\dds\\optionsBtnExpanded.dds" });

		std::vector<PosTexIndex> vertexsExpandBack = {};
		GenerateRectVertex(vertexsExpandBack, 15.0f, 2.0f, 12.0f, 12.0f, 0.0f);
		m_arrowExpandEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsExpandBack);
		m_arrowExpandEffect->addConstantBuffer<ConstantMVPIndex>();
		m_arrowExpandEffect->Init();
		categoryArrowsExpanded[categoryID] = m_arrowExpandEffect;

		std::vector<int> groupIDs = invGroupsManager::getInstance()->getAllGroupIDByCategoriesID(categoryID);
		categoryGroups[categoryID] = groupIDs;
		categoryExpanded[categoryID] = false;
	}


	categoryBgrEffect = std::make_shared<Effect>();
	categoryBgrEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	categoryBgrEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	categoryBgrEffect->addTextures({ "demoTex\\UI\\Window\\window_select.dds" });

	std::vector<PosTexIndex> vertexsCategoryBgr = {};
	GenerateRectVertex(vertexsCategoryBgr, 10.0f, 0.0f, 200.0f, 18.0f, 0.0f);
	categoryBgrEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsCategoryBgr);
	categoryBgrEffect->addConstantBuffer<ConstantMVPIndex>();
	categoryBgrEffect->Init();

	switchSelectType(3422);

	ExpandCategory(16);
}

void UIWindowSkill::ExpandCategory(int categoryID)
{
	categoryExpanded[categoryID] = true;
	for (int groupID : categoryGroups[categoryID])
	{
		groupExpanded[groupID] = false;
	}
	if (groupTexts.find(*categoryGroups[categoryID].begin()) != groupTexts.end())
		return;
	for (int groupID : categoryGroups[categoryID])
	{

		auto typeID = InvTypesManager::getInstance()->getMinTypeIdByGroupIDIcon(groupID);
		auto IconPath = getIconPathByTypeID(typeID);
		auto m_itemImgEffect = std::make_shared<Effect>();
		m_itemImgEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		m_itemImgEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
		m_itemImgEffect->addTextures({ IconPath });
		std::vector<PosTexIndex> vertexsImg = {};
		GenerateRectVertex(vertexsImg, 50.0f, 1.0f, 16.0f, 16.0f, 0.0f);
		m_itemImgEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsImg);
		m_itemImgEffect->addConstantBuffer<ConstantMVPIndex>();
		m_itemImgEffect->Init();
		groupIcons[groupID] = m_itemImgEffect;

		auto groupText = std::make_shared<UIText>();
		groupText->setSize(10, 0, 200, 20); // 初始大小设置，坐标后续在DrawUI中更新
		groupText->setText(invGroupsManager::getInstance()->getNameByGroupId(groupID));
		groupText->switchTextFormat("Bold_XS");
		groupText->Init();
		groupTexts[groupID] = groupText;

		auto m_arrowEffect = std::make_shared<Effect>();
		m_arrowEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		m_arrowEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
		m_arrowEffect->addTextures({ "demoTex\\EVE\\media\\res\\Uprising_V21.03_Icons\\Icons\\UI\\ShipUI\\dds\\optionsBtn.dds" });

		std::vector<PosTexIndex> vertexsBack = {};
		GenerateRectVertex(vertexsBack, 35.0f, 2.0f, 12.0f, 12.0f, 0.0f);
		m_arrowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsBack);
		m_arrowEffect->addConstantBuffer<ConstantMVPIndex>();
		m_arrowEffect->Init();
		groupArrows[groupID] = m_arrowEffect;

		auto m_arrowExpandEffect = std::make_shared<Effect>();
		m_arrowExpandEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		m_arrowExpandEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
		m_arrowExpandEffect->addTextures({ "demoTex\\EVE\\media\\res\\Uprising_V21.03_Icons\\Icons\\UI\\ShipUI\\dds\\optionsBtnExpanded.dds" });

		std::vector<PosTexIndex> vertexsExpandBack = {};
		GenerateRectVertex(vertexsExpandBack, 35.0f, 2.0f, 12.0f, 12.0f, 0.0f);
		m_arrowExpandEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsExpandBack);
		m_arrowExpandEffect->addConstantBuffer<ConstantMVPIndex>();
		m_arrowExpandEffect->Init();
		groupArrowsExpanded[groupID] = m_arrowExpandEffect;

		std::vector<int> typeIDs = InvTypesManager::getInstance()->getAllTypeIDByGroupID(groupID);
		groupTypes[groupID] = typeIDs;
		groupExpanded[groupID] = false;
	}
}

void UIWindowSkill::ExpandGroup(int groupID)
{
	groupExpanded[groupID] = true;
	if (groupTypes.find(groupID) != groupTypes.end() && !groupTypes[groupID].empty())
	{
		auto it = groupTypes[groupID].begin();
		if (typeTexts.find(*it) != typeTexts.end())
		{
			return;
		}
	}

	for (int typeID : groupTypes[groupID])
	{
		auto typeText = std::make_shared<UIText>();
		typeText->setSize(10, 0, 200, 20); // 初始大小设置，坐标后续在DrawUI中更新
		typeText->switchTextFormat("Bold_XS");
		typeText->setText(InvTypesManager::getInstance()->getNameByTypeId(typeID));
		typeText->Init();
		typeTexts[typeID] = typeText;
	}
}

void UIWindowSkill::CollapseCategory(int categoryID)
{
	categoryExpanded[categoryID] = false;
	for (int groupID : categoryGroups[categoryID])
	{
		groupExpanded[groupID] = false;
	}
}

void UIWindowSkill::CollapseGroup(int groupID)
{
	groupExpanded[groupID] = false;
}

void UIWindowSkill::switchSelectType(int typeID)
{
	selectTypeId = typeID;
	auto groupID = InvTypesManager::getInstance()->getGroupByTypeId(selectTypeId);
	auto groupName = invGroupsManager::getInstance()->getNameByGroupId(groupID);
	auto categoryID = invGroupsManager::getInstance()->getCategoryByGroupId(groupID);
	auto categoryName = invCategoriesManager::getInstance()->getNameByCategoriesID(categoryID);
	auto IconPath = getIconPathByTypeID(typeID);
	selectTypeImgEffect = std::make_shared<Effect>();
	selectTypeImgEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	selectTypeImgEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	selectTypeImgEffect->addTextures({ IconPath });
	std::vector<PosTexIndex> vertexsImg = {};
	GenerateRectVertex(vertexsImg, 250.0f, 50.0f, 64.0f, 64.0f, 0.0f);
	selectTypeImgEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsImg);
	selectTypeImgEffect->addConstantBuffer<ConstantMVPIndex>();
	selectTypeImgEffect->Init();

	auto name = InvTypesManager::getInstance()->getNameByTypeId(typeID);
	selectTypeNameText = std::make_shared<UIText>();
	selectTypeNameText->setSize(320.0f, 80.0f, 300.0f, 300.0f);
	selectTypeNameText->setText(name);
	selectTypeNameText->switchTextFormat("Bold_M");
	selectTypeNameText->Init();

	auto text = categoryName + std::wstring(L"/") + groupName + std::wstring(L"/");
	selectTypeGroupText = std::make_shared<UIText>();
	selectTypeGroupText->setSize(320.0f, 55.0f, 300.0f, 300.0f);
	selectTypeGroupText->setText(text);
	selectTypeGroupText->switchTextFormat("Bold_S");
	selectTypeGroupText->Init();

	auto queueTileText = L"技能队列";
	skillQueueTileText = std::make_shared<UIText>();
	skillQueueTileText->setSize(270.0f, 150.0f, 300.0f, 300.0f);
	skillQueueTileText->setText(queueTileText);
	skillQueueTileText->switchTextFormat("Bold_M");
	skillQueueTileText->Init();

	m_skillIndicator.reset();
	m_skillIndicator = GenerateSkillLevelIndicators(2, 330.0f + name.size() * 20.0f, 87.0f);

	skillAddButton.reset();
	skillAddButton = std::make_shared<UIButton>();
	skillAddButton->setSize(460.0f + name.size() * 20.0f, 82.0f, 55.0f, 22.0f);
	skillAddButton->setTex("demoTex\\UI\\Window\\window_select.dds");
	skillAddButton->setText(L"添加+");
	skillAddButton->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
	skillAddButton->Init();

	InitSkillQueue();
}

// 生成技能等级指示器
std::shared_ptr<Effect>  UIWindowSkill::GenerateSkillLevelIndicators(int level, float startX, float startY)
{
	level = std::clamp(level, 0, 5);
	float currentX = startX;

	auto effect = std::make_shared<Effect>();
	effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	effect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	effect->addTextures({ "demoTex\\UI\\Window\\window_select.dds" });

	std::vector<PosTexIndex> vertices;
	for (int i = 0; i < 5; ++i)
	{
		auto indicator = std::make_shared<Effect>();

		float size = (i < level) ? LARGE_SQUARE_SIZE : SMALL_SQUARE_SIZE;
		float yOffset = (LARGE_SQUARE_SIZE - size) / 2.0f;
		float currentY = startY + yOffset;
		GenerateRectVertex(vertices, currentX, currentY, size, size, 0.0f);

		if (i == level - 1)
			currentX += SPACING / 2.0f;
		currentX += LARGE_SQUARE_SIZE + SPACING;
	}
	effect->getVertexBuffer<PosTexIndex>()->setVertices(vertices);
	effect->addConstantBuffer<ConstantMVPIndex>();
	effect->Init();
	return effect;
}



void UIWindowSkill::InitSkillQueue()
{
	m_SkillQueueEffects.clear();
	float skillColHeight = 30.0f;
	auto pilot = SolarSystemMgr::getInstance().currentPilot;
	auto skillComp = pilot->GetComponent<SkillComponent>();
	int curY = 40;
	if (skillComp) {
		// 定义固定的文本总长度
		const int totalTextLength = 40; // 根据实际需要调整

		for (auto skill : *(skillComp->getSkillQueue())) {
			skillQueueEffect temp;
			temp.skill = skill;
			float skillPointsPerSecond = 0.4;
			int level = skill.level;
			int type = skill.skillTypeId;
			int process = skill.process;
			int remainTime = (skill.factor * 256000 * level - skill.process) / skillPointsPerSecond;
			temp.m_skillIndicator = GenerateSkillLevelIndicators(level, 655.0f, 155.0f + curY);

			std::wstring name = InvTypesManager::getInstance()->getNameByTypeId(type);

			// 1. 将数字等级转换为罗马数字
			std::wstring romanLevel = ConvertToRoman(level);

			// 2. 组合技能名称和等级
			std::wstring leftPart = name + L" " + romanLevel;

			// 3. 格式化剩余时间
			std::wstring timeStr = FormatTime(remainTime);

			// 4. 计算需要填充的空格，使总长度固定
			int spaceCount = totalTextLength - (leftPart.length() + timeStr.length());
			std::wstring spaces;
			if (spaceCount > 0) {
				spaces = std::wstring(spaceCount, L' ');
			}

			// 5. 组合最终文本
			std::wstring fullText = leftPart + spaces + timeStr;

			auto text = std::make_shared<UIText>();
			text->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
			text->setSize(270.0f, 150.0f + curY, 600.0f, 300.0f);
			text->setDelta(x, y);
			text->setText(fullText);
			text->Init();
			temp.skillNameText = text;

			m_SkillQueueEffects.push_back(temp);

			curY += skillColHeight;
		}
	}
	if (m_SkillQueueEffects.size()) {
		auto skillRemoveButton = std::make_shared<UIButton>();
		skillRemoveButton->setSize(725.0f, 145.0f + curY - skillColHeight, 22.0f, 22.0f);
		skillRemoveButton->setText(L"x");
		skillRemoveButton->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		skillRemoveButton->Init();

		auto& last = m_SkillQueueEffects.back();
		last.skillRemoveButton = skillRemoveButton;
	}
}

void UIWindowSkill::UpdateSkillQueue(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	for (auto& effect : m_SkillQueueEffects) {
		effect.skillNameText->setDelta(x, y);
		if (effect.skillRemoveButton) {
			effect.skillRemoveButton->setDelta(x, y);
			effect.skillRemoveButton->UpdateUI(dt, mouse, keyboard, tick);
			if (*(effect.skillRemoveButton->getClickFlag())) {
				auto task = std::make_shared<Task>();
				auto pilot = SolarSystemMgr::getInstance().currentPilot;
				task->publisher = pilot;
				task->target = pilot;
				(*task->paramsPtr)["taskType"] = std::string("skillComponent");
				(*task->paramsPtr)["skillTaskType"] = std::string("RemoveFromActiveQueue");
				(*task->paramsPtr)["skillTypeId"] = static_cast<int>(effect.skill.skillTypeId);
				(*task->paramsPtr)["skillLevel"] = static_cast<int>(effect.skill.level);
				TaskMgr::getInstance().addTask(task);
				*(effect.skillRemoveButton->getClickFlag()) = false;
			}
		}
	}
}

void UIWindowSkill::DrawSkillQueue()
{
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();
	XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);
	for (auto& effect : m_SkillQueueEffects) {
		effect.skillNameText->DrawUI();

		ConstantMVPIndex* dataPtr = effect.m_skillIndicator->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtr->model = XMMatrixTranspose(windowModel);
		dataPtr->view = XMMatrixTranspose(viewMatrix);
		dataPtr->projection = XMMatrixTranspose(projMatrix);
		dataPtr->TexIndex = 0;
		effect.m_skillIndicator->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		effect.m_skillIndicator->apply();
		if (effect.skillRemoveButton) {
			effect.skillRemoveButton->DrawUI();
		}
	}
}

// 辅助函数：将数字(1-5)转换为罗马数字
std::wstring UIWindowSkill::ConvertToRoman(int level)
{
	switch (level) {
	case 1: return L"I";
	case 2: return L"II";
	case 3: return L"III";
	case 4: return L"IV";
	case 5: return L"V";
	default: return L"?"; // 处理无效等级
	}
}

// 辅助函数：将秒数格式化为"X小时X分X秒"
std::wstring UIWindowSkill::FormatTime(int totalSeconds)
{
	if (totalSeconds <= 0) {
		return L"0秒";
	}

	int hours = totalSeconds / 3600;
	int minutes = (totalSeconds % 3600) / 60;
	int seconds = totalSeconds % 60;

	std::wstringstream wss;

	if (hours > 0) {
		wss << hours << L"小时";
	}
	if (minutes > 0) {
		wss << minutes << L"分";
	}
	wss << seconds << L"秒";

	return wss.str();
}