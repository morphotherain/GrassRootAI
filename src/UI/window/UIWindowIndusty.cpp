#include "UIWindowIndusty.h"
#include "UIButton.h"
#include "UIText.h"

#include "InvTypesManager.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"
#include "industryActivityMaterialsManager.h"
#include "industryActivityProductsManager.h"
#include "industryActivitiesManager.h"
#include <algorithm>
#include <sstream>

using namespace DirectX;

UIWindowIndusty::UIWindowIndusty() : UIWindow()
{
}

bool UIWindowIndusty::Init()
{
	windowTitle = L"工业";

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

void UIWindowIndusty::OnResize()
{
}

void UIWindowIndusty::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	UIWindow::UpdateUI(dt, mouse, keyboard, tick);
	for (auto& component : childComponents) {
		component->setDelta(x, y);
	}

	// 更新按钮
	if (m_startButton) {
		m_startButton->setDelta(x, y);
		m_startButton->UpdateUI(dt, mouse, keyboard, tick);
		if (*(m_startButton->getClickFlag())) {
			*(m_startButton->getClickFlag()) = false;
			if (m_selectedBlueprintTypeId != -1) {
				int timeSec = industryActivitiesManager::getInstance()->getTimeByBlueprint(m_selectedBlueprintTypeId, 1);
				if (timeSec <= 0) timeSec = 10;
				IndustryJob job{};
				job.blueprintTypeId = m_selectedBlueprintTypeId;
				job.remainingSeconds = timeSec;
				BuildQueueLine(job, static_cast<float>(150 + static_cast<int>(m_jobs.size()) * 30));
				m_jobs.push_back(job);
			}
		}
	}

	// 每秒更新一次倒计时
	if (!m_jobs.empty() && tick % 60 == 0) {
		for (auto& job : m_jobs) {
			job.remainingSeconds = max( 0, job.remainingSeconds - 1 );
			if (job.lineText) {
				std::wstring name = InvTypesManager::getInstance()->getNameByTypeId(job.blueprintTypeId);
				job.lineText->setText(name + L"  " + FormatTime(job.remainingSeconds));
			}
		}
		// 清理完成的任务
		m_jobs.erase(std::remove_if(m_jobs.begin(), m_jobs.end(), [](const IndustryJob& j) { return j.remainingSeconds <= 0; }), m_jobs.end());
	}

	// 偏移更新
	if (m_blueprintNameText) m_blueprintNameText->setDelta(x, y);
	if (m_titleLeftText) m_titleLeftText->setDelta(x, y);
	if (m_titleRightText) m_titleRightText->setDelta(x, y);
	if (m_queueTitleText) m_queueTitleText->setDelta(x, y);
	for (auto& t : m_materialTexts) t->setDelta(x, y);
	for (auto& t : m_productTexts) t->setDelta(x, y);
	for (auto& j : m_jobs) if (j.lineText) j.lineText->setDelta(x, y);
}

void UIWindowIndusty::DrawUI()
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

	// 蓝图图标
	if (m_blueprintIconEffect) {
		ConstantMVPIndex* d = m_blueprintIconEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
		d->model = XMMatrixTranspose(windowModel);
		d->view = XMMatrixTranspose(viewMatrix);
		d->projection = XMMatrixTranspose(projMatrix);
		d->TexIndex = 0;
		m_blueprintIconEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		m_blueprintIconEffect->apply();
	}
	if (m_blueprintNameText) m_blueprintNameText->DrawUI();
	if (m_titleLeftText) m_titleLeftText->DrawUI();
	if (m_titleRightText) m_titleRightText->DrawUI();
	if (m_queueTitleText) m_queueTitleText->DrawUI();

	// 材料
	for (auto& e : m_materialIconEffects) {
		ConstantMVPIndex* d = e->getConstantBuffer<ConstantMVPIndex>()->Map();
		d->model = XMMatrixTranspose(windowModel);
		d->view = XMMatrixTranspose(viewMatrix);
		d->projection = XMMatrixTranspose(projMatrix);
		d->TexIndex = 0;
		e->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		e->apply();
	}
	for (auto& t : m_materialTexts) t->DrawUI();

	// 产出
	for (auto& e : m_productIconEffects) {
		ConstantMVPIndex* d = e->getConstantBuffer<ConstantMVPIndex>()->Map();
		d->model = XMMatrixTranspose(windowModel);
		d->view = XMMatrixTranspose(viewMatrix);
		d->projection = XMMatrixTranspose(projMatrix);
		d->TexIndex = 0;
		e->getConstantBuffer<ConstantMVPIndex>()->Unmap();
		e->apply();
	}
	for (auto& t : m_productTexts) t->DrawUI();

	// 队列 + 按钮
	for (auto& j : m_jobs) if (j.lineText) j.lineText->DrawUI();
	if (m_startButton) m_startButton->DrawUI();

	for (auto& component : childComponents) {
		component->DrawUI();
	}
}

void UIWindowIndusty::cleanup()
{
}

bool UIWindowIndusty::InitResource()
{
	return true;
}

bool UIWindowIndusty::InitEffect()
{
	return true;
}


void UIWindowIndusty::ParseParameters(std::unordered_map<std::string, std::any> paras)
{
	UIWindow::setSize(300, 400, 800, 600);
}

void UIWindowIndusty::InitWindowComponent()
{
	UIWindow::InitWindowComponent();

	// 基础分割
	auto vertexs = m_windowEffect->getVertexBuffer<PosTexIndex>()->getVertices();
	GenerateRectVertex(vertexs, 250.0f, 45.0f, width - 250.0f, 1.0f, 7.0f);
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexs);
	m_windowEffect->addBlendState();
	m_windowEffect->Init();

	m_titleLeftText = std::make_shared<UIText>();
	m_titleLeftText->setSize(20.0f, 50.0f, 300.0f, 300.0f);
	m_titleLeftText->setText(L"材料");
	m_titleLeftText->switchTextFormat("Bold_M");
	m_titleLeftText->Init();

	m_titleRightText = std::make_shared<UIText>();
	m_titleRightText->setSize(600.0f, 50.0f, 300.0f, 300.0f);
	m_titleRightText->setText(L"产出");
	m_titleRightText->switchTextFormat("Bold_M");
	m_titleRightText->Init();

	m_queueTitleText = std::make_shared<UIText>();
	m_queueTitleText->setSize(270.0f, 150.0f, 300.0f, 300.0f);
	m_queueTitleText->setText(L"制造队列");
	m_queueTitleText->switchTextFormat("Bold_M");
	m_queueTitleText->Init();

	m_startButton = std::make_shared<UIButton>();
	m_startButton->setSize(400.0f, 125.0f, 80.0f, 26.0f);
	m_startButton->setText(L"开始制造");
	m_startButton->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
	m_startButton->Init();
	m_startButton->setDelta(x, y);

}

void UIWindowIndusty::LoadBlueprint(int blueprintTypeId)
{
	m_selectedBlueprintTypeId = blueprintTypeId;
	BuildCenterUI(blueprintTypeId);
	BuildMaterialsUI(blueprintTypeId);
	BuildProductsUI(blueprintTypeId);
}

void UIWindowIndusty::BuildCenterUI(int blueprintTypeId)
{
	auto IconPath = getIconPathByTypeID(blueprintTypeId);
	m_blueprintIconEffect = std::make_shared<Effect>();
	m_blueprintIconEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_blueprintIconEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_blueprintIconEffect->addTextures({ IconPath });
	std::vector<PosTexIndex> vertexsImg = {};
	GenerateRectVertex(vertexsImg, 350.0f, 50.0f, 64.0f, 64.0f, 0.0f);
	m_blueprintIconEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertexsImg);
	m_blueprintIconEffect->addConstantBuffer<ConstantMVPIndex>();
	m_blueprintIconEffect->Init();

	auto name = InvTypesManager::getInstance()->getNameByTypeId(blueprintTypeId);
	m_blueprintNameText = std::make_shared<UIText>();
	m_blueprintNameText->setSize(420.0f, 80.0f, 300.0f, 300.0f);
	m_blueprintNameText->setText(name);
	m_blueprintNameText->switchTextFormat("Bold_M");
	m_blueprintNameText->Init();
}

void UIWindowIndusty::BuildMaterialsUI(int blueprintTypeId)
{
	m_materialIconEffects.clear();
	m_materialTexts.clear();
	auto mats = industryActivityMaterialsManager::getInstance()->getMaterialsByBlueprint(blueprintTypeId, 1);
	float curY = 90.0f;
	for (auto& m : mats) {
		auto iconPath = getIconPathByTypeID(m.materialTypeID);
		auto effect = std::make_shared<Effect>();
		effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		effect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
		effect->addTextures({ iconPath });
		std::vector<PosTexIndex> verts = {};
		GenerateRectVertex(verts, 20.0f, curY, 32.0f, 32.0f, 0.0f);
		effect->getVertexBuffer<PosTexIndex>()->setVertices(verts);
		effect->addConstantBuffer<ConstantMVPIndex>();
		effect->Init();
		m_materialIconEffects.push_back(effect);

		auto text = std::make_shared<UIText>();
		text->setSize(60.0f, curY + 6.0f, 350.0f, 350.0f);
		auto name = InvTypesManager::getInstance()->getNameByTypeId(m.materialTypeID);
		text->setText(name + L" x" + std::to_wstring(m.quantity));
		text->switchTextFormat("Bold_S");
		text->Init();
		m_materialTexts.push_back(text);

		curY += 36.0f;
	}
}

void UIWindowIndusty::BuildProductsUI(int blueprintTypeId)
{
	m_productIconEffects.clear();
	m_productTexts.clear();
	auto products = industryActivityProductsManager::getInstance()->getProductsByBlueprint(blueprintTypeId, 1);
	float baseX = 600.0f;
	float curY = 90.0f;
	for (auto& p : products) {
		auto iconPath = getIconPathByTypeID(p.productTypeID);
		auto effect = std::make_shared<Effect>();
		effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
		effect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
		effect->addTextures({ iconPath });
		std::vector<PosTexIndex> verts = {};
		GenerateRectVertex(verts, baseX, curY, 32.0f, 32.0f, 0.0f);
		effect->getVertexBuffer<PosTexIndex>()->setVertices(verts);
		effect->addConstantBuffer<ConstantMVPIndex>();
		effect->Init();
		m_productIconEffects.push_back(effect);

		auto text = std::make_shared<UIText>();
		text->setSize(baseX + 40.0f, curY + 6.0f, 350.0f, 350.0f);
		auto name = InvTypesManager::getInstance()->getNameByTypeId(p.productTypeID);
		std::wstring right = name + L" x" + std::to_wstring(p.quantity);
		text->setText(right);
		text->switchTextFormat("Bold_S");
		text->Init();
		m_productTexts.push_back(text);

		curY += 36.0f;
	}
}

void UIWindowIndusty::BuildQueueLine(IndustryJob& job, float offsetY)
{
	auto text = std::make_shared<UIText>();
	text->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
	text->setSize(270.0f, offsetY, 600.0f, 300.0f);
	text->setDelta(x, y);
	auto name = InvTypesManager::getInstance()->getNameByTypeId(job.blueprintTypeId);
	text->setText(name + L"  " + FormatTime(job.remainingSeconds));
	text->Init();
	job.lineText = text;
}

std::wstring UIWindowIndusty::FormatTime(int totalSeconds)
{
	if (totalSeconds <= 0) return L"0秒";
	int hours = totalSeconds / 3600;
	int minutes = (totalSeconds % 3600) / 60;
	int seconds = totalSeconds % 60;
	std::wstringstream wss;
	if (hours > 0) wss << hours << L"小时";
	if (minutes > 0) wss << minutes << L"分";
	wss << seconds << L"秒";
	return wss.str();
}

void UIWindowIndusty::handleTask(Task& task)
{
	// 允许外部任务设置当前蓝图
	if (task.paramsPtr->find("blueprintTypeID") != task.paramsPtr->end()) {
		int typeId = task.getParamOrDefault<int>("blueprintTypeID", -1);
		if (typeId != -1) {
			LoadBlueprint(typeId);
		}
	}
}
