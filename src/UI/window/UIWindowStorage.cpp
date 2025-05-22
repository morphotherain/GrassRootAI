#include "UIWindowStorage.h"
#include "UIButton.h"
#include "UIText.h"

#include "InvTypesManager.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"

using namespace DirectX;

UIWindowStorage::UIWindowStorage() : UIWindow()
{
}

bool UIWindowStorage::Init()
{
	windowTitle = L"仓库";

	InitWindowComponent();

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& component : childComponents) {
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}

	rowHeight = 90.0f;
	colWidth = 76.0f;

	auto currentPilot = SolarSystemMgr::getInstance().currentPilot;
	auto currentShip = currentPilot->currentShip;
	auto base = currentShip->GetComponent<BaseComponent>();
	auto cargo = currentShip->GetComponent<CargoContainerComponent>();
	auto typeName = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
	currentContainerID = dynContainersManager::getInstance()->getContainerID(base->objectID, CONTAINER_TYPE_CARGO);
	
	InitItemImgEffect();

	currentContainerTitle->setText(typeName);

	shipCargo = std::make_shared<Container>();
	shipCargo->text->setText(typeName);
	shipCargo->text->setSize(20.0f, TitleHeight + 32.0f, 350.0f, 350.0f);
	shipCargo->Init();


	return false;
}

void UIWindowStorage::OnResize()
{
}

void UIWindowStorage::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	UIWindow::UpdateUI(dt, mouse, keyboard, tick);

	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);
	// 检测鼠标左键是否刚被按下（上升沿检测）
	bool leftButtonPressed = mouseState.leftButton && !lastMouseState.leftButton;
	if (leftButtonPressed) {
		if ((x + 220) < mouseState.x && (x + 800) > mouseState.x && (y + 600) > mouseState.y && (y + 100) < mouseState.y)
		{
			int offset_x = (mouseState.x - 220 - x) / colWidth;
			int offset_y = (mouseState.y - 100 - y) / rowHeight;
			dragItemIndex = offset_y * 7 + offset_x;
			if (dragItemIndex >= m_itemsPair.size())dragItemIndex = -1;
			InitDragItemEffect();
			InitItemImgEffect();
		}
	}
	if (dragItemIndex != -1) {
		HandleMouseDrag(dt, mouse);
	}
	if (!mouseState.leftButton && dragItemIndex != -1) {
		OnDragEnd(mouseState.x, mouseState.y);
		dragItemIndex = -1;
		dragItemOffsetY = 0;
		dragItemOffsetX = 0;
		InitItemImgEffect();
	}

	for (auto& component : childComponents) {
		component->setDelta(x, y);
	}
	for (auto& text : m_itemsTexts) {
		text->setDelta(x, y + offsetY);
	}

	if(dragItemIndex != -1)
	{
		dragItemText->setDelta(x + dragItemOffsetX, y + offsetY + dragItemOffsetY);
		dragItemNumText->setDelta(x + dragItemOffsetX, y + offsetY + dragItemOffsetY);
	}



	
	// 在鼠标没进入窗口前仍为ABSOLUTE
	if ((x + 200) < mouseState.x && (x + 800) > mouseState.x && (y + 600) > mouseState.y && y < mouseState.y)
	{
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


	shipCargo->text->setDelta(x, y);
}

void UIWindowStorage::DrawUI()
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
	float adjustedX = 568.0f;
	float adjustedY = 234.0f;
	D3D11_VIEWPORT adjustedViewport = originalViewport;
	adjustedViewport.TopLeftX = x;
	adjustedViewport.TopLeftY = 50.0f + y;
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
	XMMATRIX adjustedWindowModel = XMMatrixTranslation(adjustedX, adjustedY + offsetY, 0.0f);


	ConstantMVPIndex* dataPtritemImgBack = m_itemBackEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtritemImgBack->model = XMMatrixTranspose(adjustedWindowModel);
	dataPtritemImgBack->view = XMMatrixTranspose(viewMatrix);
	dataPtritemImgBack->projection = XMMatrixTranspose(adjustedProjectionMatrix);
	dataPtritemImgBack->TexIndex = 0;
	m_itemBackEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_itemBackEffect->apply();

	for (auto itemImgEffect : m_itemImgEffects) {
		ConstantMVPIndex* dataPtritemImg = itemImgEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtritemImg->model = XMMatrixTranspose(adjustedWindowModel);
		dataPtritemImg->view = XMMatrixTranspose(viewMatrix);
		dataPtritemImg->projection = XMMatrixTranspose(adjustedProjectionMatrix);
		itemImgEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		itemImgEffect->apply();
	}

	ConstantMVPIndex* dataPtritemNumBack = m_itemNumBackEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtritemNumBack->model = XMMatrixTranspose(adjustedWindowModel);
	dataPtritemNumBack->view = XMMatrixTranspose(viewMatrix);
	dataPtritemNumBack->projection = XMMatrixTranspose(adjustedProjectionMatrix);
	dataPtritemNumBack->TexIndex = 0;
	m_itemNumBackEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_itemNumBackEffect->apply();

	m_pd3dImmediateContext->RSSetViewports(1, &originalViewport);

	if (dragItemIndex != -1) {
		XMMATRIX windowModelDrag = XMMatrixTranslation(x + dragItemOffsetX, y + dragItemOffsetY, 0.0f);
		ConstantMVPIndex* dataPtritemImgBack = dragItemBackEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtritemImgBack->model = XMMatrixTranspose(windowModelDrag);
		dataPtritemImgBack->view = XMMatrixTranspose(viewMatrix);
		dataPtritemImgBack->projection = XMMatrixTranspose(projMatrix);
		dataPtritemImgBack->TexIndex = 0;
		dragItemBackEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		dragItemBackEffect->apply();

		ConstantMVPIndex* dataPtritemImg = dragItemImgEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtritemImg->model = XMMatrixTranspose(windowModelDrag);
		dataPtritemImg->view = XMMatrixTranspose(viewMatrix);
		dataPtritemImg->projection = XMMatrixTranspose(projMatrix);
		dragItemImgEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		dragItemImgEffect->apply();

		ConstantMVPIndex* dataPtritemNumBack = dragItemNumBackEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtritemNumBack->model = XMMatrixTranspose(windowModelDrag);
		dataPtritemNumBack->view = XMMatrixTranspose(viewMatrix);
		dataPtritemNumBack->projection = XMMatrixTranspose(projMatrix);
		dataPtritemNumBack->TexIndex = 0;
		dragItemNumBackEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		dragItemNumBackEffect->apply();

		dragItemText->DrawUI();
		dragItemNumText->DrawUI();
	}

	for (auto& text : m_itemsTexts) {
		text->DrawUI();
	}

	for (auto& component : childComponents) {
		component->DrawUI();
	}
	shipCargo->Draw();
}

void UIWindowStorage::cleanup()
{
}

bool UIWindowStorage::InitResource()
{
	return true;
}

bool UIWindowStorage::InitEffect()
{
	return true;
}

bool UIWindowStorage::InitItemImgEffect()
{
	m_itemsTexts.clear();
	m_itemImgEffects.clear();
	m_itemsPair.clear();

	if (dynGameObjectsManager::getInstance()->queryObjectsByContainerID(currentContainerID, m_itemsPair)) {
		DEBUG_("查询到的结果数量: {}", m_itemsPair.size());
		for (const auto& pair : m_itemsPair) {
			DEBUG_("ObjectID: {}, TypeID: {}", pair.first, pair.second);
		}
	}
	else {
		DEBUG_("查询失败");
	}
	std::vector<PosTexIndex> vertexsBack = {};
	std::vector<PosTexIndex> vertexsNumBack = {};

	m_itemBackEffect = std::make_shared<Effect>();
	m_itemBackEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_itemBackEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_itemBackEffect->addTextures({ "demoTex\\UI\\Window\\Item_back.dds" });

	m_itemNumBackEffect = std::make_shared<Effect>();
	m_itemNumBackEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_itemNumBackEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_itemNumBackEffect->addTextures({ "demoTex\\UI\\Window\\window_black.dds" });

	float index = -1.0f;
	float row = 0.0f;
	int count = -1;

	for (const auto& pair : m_itemsPair) {

		index += 1.0f;
		if (index > 6.0f) {
			index = 0.0f;
			row += 1.0f;
		}
		count++;
		if (count== dragItemIndex)
			continue;
		m_itemImgEffect = std::make_shared<Effect>();
		m_itemImgEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		m_itemImgEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");


		auto quantity = 1;
		auto object = SolarSystemMgr::getInstance().getObjectById(pair.first);
		if(object && object->GetComponent<AttributesComponent>()){
			auto attributes = object->GetComponent<AttributesComponent>();
			quantity = (*attributes->objectAttributes)[ATTR_ID_QUANTITY].value;
		}
		else {
			if (object) 
				DEBUG_("Couldn't load Attributes objectID {}", pair.first);
			else
				DEBUG_("Couldn't load objectID {}", pair.first);
		}
		DEBUG_("Couldn't load objectID {}", pair.first);

		std::string IconPath = getIconPathByTypeID(pair.second);
		std::wstring name = InvTypesManager::getInstance()->getNameByTypeId(pair.second);
		std::wstring processedName = L"";
		for (size_t i = 0; i < name.length(); ++i) {
			if (i > 0 && i % 5 == 0) {
				processedName += L'\n';
			}
			processedName += name[i];
		}
		DEBUG_("ObjectID: {}, TypeID: {}, IconPath:{}", pair.first, pair.second, IconPath);

		float offsetY_ = row * rowHeight;
		std::vector<PosTexIndex> vertexsImg = {};
		GenerateRectVertex(vertexsImg, 220.0f + colWidth * index, 100.0f + offsetY_, 64.0f, 64.0f, 0);
		GenerateRectVertex(vertexsBack, 220.0f + colWidth * index, 100.0f + offsetY_, 64.0f, 64.0f, 0.0f);
		GenerateRectVertex(vertexsNumBack, 256.0f + colWidth * index, 150.0f + offsetY_, 30.0f, 16.0f, 0.0f);

		auto textName = std::make_unique<UIText>();
		textName->setSize(220.0f + colWidth * index, 170.0f + offsetY_, 350.0f, 350.0f);
		textName->setText(processedName);
		textName->switchTextFormat("Bold_XS");
		m_itemsTexts.push_back(std::move(textName));

		auto textNum = std::make_unique<UIText>();
		textNum->setSize(260.0f + colWidth * index, 147.0f + offsetY_, 350.0f, 350.0f);
		textNum->setText(std::to_wstring(quantity));
		m_itemsTexts.push_back(std::move(textNum));


		m_itemImgEffect->addTextures({ IconPath });
		m_itemImgEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsImg);
		m_itemImgEffect->addConstantBuffer<ConstantMVPIndex>();
		m_itemImgEffect->Init();
		m_itemImgEffects.push_back(m_itemImgEffect);
	}
	m_itemBackEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsBack);
	m_itemBackEffect->addConstantBuffer<ConstantMVPIndex>();
	m_itemBackEffect->Init();
	m_itemNumBackEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsNumBack);
	m_itemNumBackEffect->addConstantBuffer<ConstantMVPIndex>();
	m_itemNumBackEffect->Init();

	for (auto& text : m_itemsTexts) {
		text->Init();
	}

	return true;
}

void UIWindowStorage::InitDragItemEffect()
{
	if (dragItemIndex >= m_itemsPair.size())return;
	float index = (dragItemIndex % 7);
	float row = (dragItemIndex / 7);
	auto pair = m_itemsPair[dragItemIndex];
	int count = -1;
	std::vector<PosTexIndex> vertexsBack = {};
	std::vector<PosTexIndex> vertexsNumBack = {};


	auto quantity = 1;
	auto object = SolarSystemMgr::getInstance().getObjectById(pair.first);
	if (object && object->GetComponent<AttributesComponent>()) {
		auto attributes = object->GetComponent<AttributesComponent>();
		quantity = (*attributes->objectAttributes)[ATTR_ID_QUANTITY].value;
	}

	std::string IconPath = getIconPathByTypeID(pair.second);
	std::wstring name = InvTypesManager::getInstance()->getNameByTypeId(pair.second);
	std::wstring processedName = L"";
	for (size_t i = 0; i < name.length(); ++i) {
		if (i > 0 && i % 5 == 0) {
			processedName += L'\n';
		}
		processedName += name[i];
	}
	DEBUG_("ObjectID: {}, TypeID: {}, IconPath:{}", pair.first, pair.second, IconPath);

	float offsetY_ = row * rowHeight;
	std::vector<PosTexIndex> vertexsImg = {};
	GenerateRectVertex(vertexsImg, 220.0f + colWidth * index, 100.0f + offsetY_, 64.0f, 64.0f, 0);
	GenerateRectVertex(vertexsBack, 220.0f + colWidth * index, 100.0f + offsetY_, 64.0f, 64.0f, 0.0f);
	GenerateRectVertex(vertexsNumBack, 256.0f + colWidth * index, 150.0f + offsetY_, 30.0f, 16.0f, 0.0f);

	dragItemText = std::make_unique<UIText>();
	dragItemText->setSize(220.0f + colWidth * index, 170.0f + offsetY_, 350.0f, 350.0f);
	dragItemText->setText(processedName);
	dragItemText->switchTextFormat("Bold_XS");
	dragItemText->Init();

	dragItemNumText = std::make_unique<UIText>();
	dragItemNumText->setSize(260.0f + colWidth * index, 147.0f + offsetY_, 350.0f, 350.0f);
	dragItemNumText->setText(std::to_wstring(quantity));
	dragItemNumText->Init();

	dragItemImgEffect = std::make_shared<Effect>();
	dragItemImgEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	dragItemImgEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	dragItemImgEffect->addTextures({ IconPath });
	dragItemImgEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsImg);
	dragItemImgEffect->addConstantBuffer<ConstantMVPIndex>();
	dragItemImgEffect->Init();


	dragItemBackEffect = std::make_shared<Effect>();
	dragItemBackEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	dragItemBackEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	dragItemBackEffect->addTextures({ "demoTex\\UI\\Window\\Item_back_select.dds" });
	dragItemBackEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsBack);
	dragItemBackEffect->addConstantBuffer<ConstantMVPIndex>();
	dragItemBackEffect->Init();

	dragItemNumBackEffect = std::make_shared<Effect>();
	dragItemNumBackEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	dragItemNumBackEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	dragItemNumBackEffect->addTextures({ "demoTex\\UI\\Window\\window_black.dds" });
	dragItemNumBackEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsNumBack);
	dragItemNumBackEffect->addConstantBuffer<ConstantMVPIndex>();
	dragItemNumBackEffect->Init();
}

void UIWindowStorage::DrawDragItem()
{
}

void UIWindowStorage::HandleMouseDrag(float dt, DirectX::Mouse& mouse)
{
	m_inputHandler.Update(dt, mouse.GetState());

	m_inputHandler.SetDetectionArea(x, y, width, height);

	if (m_inputHandler.IsMouseDragging()) {
		int dx, dy;
		m_inputHandler.GetDragDelta(dx, dy);
		dragItemOffsetX += dx;
		dragItemOffsetY += dy;
	}
}

void UIWindowStorage::ParseParameters(std::unordered_map<std::string, std::any> paras)
{
	x = 300;
	y = 400;
	deltaX = 800;
	deltaY = 600;
	UIWindow::setSize(x, y, deltaX, deltaY);
}

void UIWindowStorage::OnDragEnd(int x, int y)
{
	std::shared_ptr<Task> task = std::make_shared<Task>();
	task->targetSystem = UIWINDOW;
	(*task->paramsPtr)["x"] = x;
	(*task->paramsPtr)["y"] = y;
	(*task->paramsPtr)["ItemDrag"] = 1;
	(*task->paramsPtr)["objectID"] = m_itemsPair[dragItemIndex].first;

	TaskMgr::getInstance().addTask(task);

}

void UIWindowStorage::InitWindowComponent()
{
	UIWindow::InitWindowComponent();

	auto vertexs = m_windowEffect->getVertexBuffer<PosTexIndex>()->getVertices();
	GenerateRectVertex(vertexs, 200.0f, TitleHeight, 5.0f, 600.0f - TitleHeight - 1.0f, 5.0f);
	GenerateRectVertex(vertexs, 200.0f, TitleHeight + 25.0f, 600.0f, 3.0f, 5.0f);
	GenerateRectVertex(vertexs, 200.0f, TitleHeight + 50.0f, 600.0f, 3.0f, 5.0f);
	GenerateRectVertex(vertexs, 3.0f, TitleHeight + 30.0f, 195.0f, 20.0f, 6.0f);
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexs);
	m_windowEffect->addBlendState();
	m_windowEffect->Init();

	auto text = std::make_shared<UIText>();
	text->setSize(3.0f, TitleHeight + 5.0f, 350.0f, 350.0f);
	text->setText(L"目录");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(213.0f, TitleHeight + 2.0f, 350.0f, 350.0f);
	text->setText(L"");
	currentContainerTitle = text;
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(493.0f, TitleHeight + 2.0f + 25.0f, 350.0f, 350.0f);
	text->setText(L"0.0/0.0 m³");
	currentContainerUsed = text;
	AddUIComponent(text);
}

UIWindowStorage::Container::Container()
{
	text = std::make_shared<UIText>();
}

void UIWindowStorage::Container::Init()
{
	text->Init();
}

void UIWindowStorage::Container::Update()
{
	text->setDelta(level * 20.0f, index * 20.0f);
}

void UIWindowStorage::Container::Draw()
{
	text->DrawUI();
}