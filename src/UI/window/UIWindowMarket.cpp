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
	windowTitle = L"星域市场";

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
	if (tick % 100 == 0)
	{
		// 更新卖出订单表格数据
		updateTable(m_sellTable, m_sellOrders, m_sellTableFirstDataRow, true);
		// 更新买入订单表格数据
		updateTable(m_buyTable, m_buyOrders, m_buyTableFirstDataRow, false);
	}

	m_sellTable.setDelta(x, y);
	m_buyTable.setDelta(x, y);
	m_sellTable.UpdateUI(dt, mouse, keyboard, tick);
	m_buyTable.UpdateUI(dt, mouse, keyboard, tick);

}

void UIWindowMarket::DrawUI()
{
	DrawIndex();
	DrawInfo();

	// 绘制卖出和买入订单表格
	m_sellTable.DrawUI();
	m_buyTable.DrawUI();
}

void UIWindowMarket::cleanup()
{
}

void UIWindowMarket::DrawIndex()
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
	adjustedViewport.TopLeftY = y+50.0f;
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
							XMMATRIX windowModelSelectTypeBgr= XMMatrixTranslation(adjustedX, currentY - y + adjustedY, 0.0f);
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

void UIWindowMarket::DrawInfo()
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
	selectTypeGroupText->setDelta(x, y);
	selectTypeGroupText->DrawUI();
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

	auto vertexs = m_windowEffect->getVertexBuffer<PosTexIndex>()->getVertices();
	GenerateRectVertex(vertexs, 0.0f, 45.0f, 230.0f, 1.0f, 7.0f);
	GenerateRectVertex(vertexs, 250.0f, 45.0f , deltaX-250.0f, 1.0f, 7.0f);
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexs);
	m_windowEffect->addBlendState();
	m_windowEffect->Init();

	std::vector<int> categoryIDs = {4,5,6,7,8,9, 16,17,18,20,22,23,24,32,40,41,42,63,65,66,87,91};

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

	switchSelectType(621);

	// ---------------------- 初始化卖出订单表格 ----------------------
	m_sellTable.setPosition(250, 150); // 设置表格位置
	m_sellTable.setColumnWidth(0, 40);         // 跳跃
	m_sellTable.setColumnWidth(1, 40);          // 数量
	m_sellTable.setColumnWidth(2, 120);          // 价格
	m_sellTable.setColumnWidth(3, 200);           // 地点
	m_sellTable.setColumnWidth(4, 150);          // 距到期还有
	// 添加表头
	m_sellTable.setCellText(0, 0, L"跳跃", 12.0f, D2D1::ColorF::LightGray);
	m_sellTable.setCellText(0, 1, L"数量", 12.0f, D2D1::ColorF::LightGray);
	m_sellTable.setCellText(0, 2, L"价格", 12.0f, D2D1::ColorF::LightGray);
	m_sellTable.setCellText(0, 3, L"地点", 12.0f, D2D1::ColorF::LightGray);
	m_sellTable.setCellText(0, 4, L"距到期还有", 12.0f, D2D1::ColorF::LightGray);

	// ---------------------- 初始化买入订单表格 ----------------------
	m_buyTable.setPosition(250, 350);
	m_buyTable.setColumnWidth(0, 40);
	m_buyTable.setColumnWidth(1, 40);
	m_buyTable.setColumnWidth(2, 120);
	m_buyTable.setColumnWidth(3, 200);
	m_buyTable.setColumnWidth(4, 40);
	m_buyTable.setColumnWidth(5, 150);

	m_buyTable.setCellText(0, 0, L"跳跃", 12.0f, D2D1::ColorF::LightGray);
	m_buyTable.setCellText(0, 1, L"数量", 12.0f, D2D1::ColorF::LightGray);
	m_buyTable.setCellText(0, 2, L"价格", 12.0f, D2D1::ColorF::LightGray);
	m_buyTable.setCellText(0, 3, L"地点", 12.0f, D2D1::ColorF::LightGray);
	m_buyTable.setCellText(0, 4, L"范围", 12.0f, D2D1::ColorF::LightGray);
	m_buyTable.setCellText(0, 5, L"距到期还有", 12.0f, D2D1::ColorF::LightGray);

}

void UIWindowMarket::ExpandCategory(int categoryID)
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

void UIWindowMarket::ExpandGroup(int groupID)
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

void UIWindowMarket::switchSelectType(int typeID)
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
	selectTypeNameText->setSize(320.0f,80.0f,300.0f,300.0f);
	selectTypeNameText->setText(name);
	selectTypeNameText->switchTextFormat("Bold_M");
	selectTypeNameText->Init();
	
	auto text = categoryName + std::wstring(L"/") + groupName + std::wstring(L"/");
	selectTypeGroupText = std::make_shared<UIText>();
	selectTypeGroupText->setSize(320.0f, 55.0f, 300.0f, 300.0f);
	selectTypeGroupText->setText(text);
	selectTypeGroupText->switchTextFormat("Bold_S");
	selectTypeGroupText->Init();


	// 填充当前类型的订单数据
	auto solarSystem = SolarSystemMgr::getInstance().currentSolarSystem;
	auto regionID = solarSystem->getRegionID();
	fillOrders(typeID, regionID);
}

void UIWindowMarket::updateTable(UITable& table, std::vector<MarketOrder>& orders, int firstDataRow, bool isSellOrder)
{
	int row = firstDataRow;
	for (auto& order : orders) {
		// 清空旧数据
		for (int col = 0; col < 5; ++col) {
			table.setCellText(row, col, L"");
		}

		// 格式化价格为两位小数
		std::wstringstream priceStream;
		priceStream.precision(2);
		priceStream << std::fixed << order.price;
		std::wstring formattedPrice = priceStream.str();



		// 填充新数据
		table.setCellText(row, 0, L"空间站", 12.0f, D2D1::ColorF::White);
		table.setCellText(row, 1, std::to_wstring(order.amount), 12.0f, D2D1::ColorF::White);
		table.setCellText(row, 2, formattedPrice, 12.0f,D2D1::ColorF::White);
		table.setCellText(row, 3, order.locationObjectIDText , 12.0f, D2D1::ColorF::LightGray);
		if (isSellOrder) {
			table.setCellText(row, 4, std::to_wstring(order.ticksRemain), 12.0f, D2D1::ColorF::LightGray);
		}
		else {
			if(order.availbleRange == 0)
				table.setCellText(row, 4, L"空间站", 12.0f, D2D1::ColorF::LightGray);
			else {
				table.setCellText(row, 4, std::to_wstring(order.availbleRange)+L"跳", 12.0f, D2D1::ColorF::LightGray);
			}
			table.setCellText(row, 5, std::to_wstring(order.ticksRemain), 12.0f, D2D1::ColorF::LightGray);
		}

		// 设置单元格用户数据为orderID
		table.setCellUserData(row, 0, order.orderID); // 假设同一行所有单元格的userData相同
		table.setCellUserData(row, 1, order.orderID);
		table.setCellUserData(row, 2, order.orderID);
		table.setCellUserData(row, 3, order.orderID);
		table.setCellUserData(row, 4, order.orderID);

		row++;
	}
}

void UIWindowMarket::fillTestOrders(int typeID)
{
	auto locationObjectID = 60012145;
	auto object = GameObjectMgr::getInstance().getObject(locationObjectID);
	auto locationName = object->GetComponent<BaseComponent>()->name;
	// 卖出订单示例
	MarketOrder sellOrder1;
	sellOrder1.orderID = 1001;
	sellOrder1.amount = 5;
	sellOrder1.price = 500000.0;
	sellOrder1.locationObjectID = locationObjectID;
	sellOrder1.locationObjectIDText = locationName;
	sellOrder1.ticksRemain = 30;
	sellOrder1.isSellOrder = true;
	m_sellOrders.push_back(sellOrder1);

	// 买入订单示例
	MarketOrder buyOrder1;
	buyOrder1.orderID = 2001;
	buyOrder1.amount = 10;
	buyOrder1.price = 480000.0;
	buyOrder1.locationObjectID = locationObjectID;
	buyOrder1.locationObjectIDText = locationName;
	buyOrder1.ticksRemain = 15;
	buyOrder1.isSellOrder = false;
	m_buyOrders.push_back(buyOrder1);
	
}

void UIWindowMarket::fillOrders(int typeID, int regionID) {
	// 清空现有订单
	m_sellOrders.clear();
	m_buyOrders.clear();

	// 从数据库获取订单
	dynMarketOrderManager* orderMgr = dynMarketOrderManager::getInstance();
	std::vector<MarketOrder> allOrders = orderMgr->getOrdersByTypeIDandRegionID(typeID, regionID);

	// 根据订单类型分类
	for (auto& order : allOrders) {

		auto object = GameObjectMgr::getInstance().getObject(order.locationObjectID);
		order.locationObjectIDText = object->GetComponent<BaseComponent>()->name;

		if (order.isSellOrder) {
			m_sellOrders.push_back(order);
		}
		else {
			m_buyOrders.push_back(order);
		}
	}

	// 可选：按价格排序
	std::sort(m_sellOrders.begin(), m_sellOrders.end(),
		[](const MarketOrder& a, const MarketOrder& b) {
			return a.price < b.price; // 卖单按价格升序
		});

	std::sort(m_buyOrders.begin(), m_buyOrders.end(),
		[](const MarketOrder& a, const MarketOrder& b) {
			return a.price > b.price; // 买单按价格降序
		});
}