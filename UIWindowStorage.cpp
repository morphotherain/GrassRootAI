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

	auto currentPilot = SolarSystemMgr::getInstance().currentPilot;
	auto currentShip = currentPilot->currentShip;
	auto base = currentShip->GetComponent<BaseComponent>();
	auto cargo = currentShip->GetComponent<CargoContainerComponent>();
	auto typeName = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
	auto containerID = dynContainersManager::getInstance()->getContainerID(base->objectID, CONTAINER_TYPE_CARGO);
	if (dynGameObjectsManager::getInstance()->queryObjectsByContainerID(containerID, m_itemsPair)) {
		DEBUG_("查询到的结果数量: {}" , m_itemsPair.size());
		for (const auto& pair : m_itemsPair) {
			DEBUG_("ObjectID: {}, TypeID: {}" , pair.first ,  pair.second);
		}
	}
	else {
		DEBUG_("查询失败");
	}
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
	for (auto& component : childComponents) {
		component->setDelta(x, y);
	}
	for (auto& text : m_itemsTexts) {
		text->setDelta(x, y);
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

	ConstantMVPIndex* dataPtritemImgBack = m_itemBackEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtritemImgBack->model = XMMatrixTranspose(windowModel);
	dataPtritemImgBack->view = XMMatrixTranspose(viewMatrix); 
	dataPtritemImgBack->projection = XMMatrixTranspose(projMatrix);
	dataPtritemImgBack->TexIndex = 0;
	m_itemBackEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_itemBackEffect->apply();

	for (auto itemImgEffect : m_itemImgEffects) {
		ConstantMVPIndex* dataPtritemImg = itemImgEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
		dataPtritemImg->model = XMMatrixTranspose(windowModel);
		dataPtritemImg->view = XMMatrixTranspose(viewMatrix);
		dataPtritemImg->projection = XMMatrixTranspose(projMatrix);
		itemImgEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		itemImgEffect->apply();
	}

	ConstantMVPIndex* dataPtritemNumBack = m_itemNumBackEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtritemNumBack->model = XMMatrixTranspose(windowModel);
	dataPtritemNumBack->view = XMMatrixTranspose(viewMatrix);
	dataPtritemNumBack->projection = XMMatrixTranspose(projMatrix);
	dataPtritemNumBack->TexIndex = 0;
	m_itemNumBackEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_itemNumBackEffect->apply();


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

	float index = 0.0f;

	for (const auto& pair : m_itemsPair) {
		m_itemImgEffect = std::make_shared<Effect>();
		m_itemImgEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		m_itemImgEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");

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

		std::vector<PosTexIndex> vertexsImg = {};
		GenerateRectVertex(vertexsImg, 220.0f + 76.0f * index, 100.0f, 64.0f, 64.0f, 0);
		GenerateRectVertex(vertexsBack, 220.0f + 76.0f * index, 100.0f, 64.0f, 64.0f, 0.0f);
		GenerateRectVertex(vertexsNumBack, 256.0f + 76.0f * index, 150.0f, 30.0f, 16.0f, 0.0f);

		auto textName = std::make_unique<UIText>();
		textName->setSize(220.0f + 76.0f * index, 170.0f, 350.0f,350.0f);
		textName->setText(processedName);
		textName->switchTextFormat("Arial_XS");
		m_itemsTexts.push_back(std::move(textName));

		auto textNum = std::make_unique<UIText>();
		textNum->setSize(260.0f + 76.0f * index, 147.0f, 350.0f, 350.0f);
		textNum->setText(L"1");
		m_itemsTexts.push_back(std::move(textNum));


		index += 1.0f;

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

void UIWindowStorage::ParseParameters(std::unordered_map<std::string, std::any> paras)
{
	x = 300;
	y = 400;
	deltaX = 800;
	deltaY = 600;
	UIWindow::setSize(x, y, deltaX, deltaY);
}

void UIWindowStorage::InitWindowComponent()
{
	UIWindow::InitWindowComponent();

	auto vertexs = m_windowEffect->getVertexBuffer<PosTexIndex>()->getVertices();
	GenerateRectVertex(vertexs, 200.0f , TitleHeight, 5.0f, 600.0f - TitleHeight -1.0f, 5.0f);
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
