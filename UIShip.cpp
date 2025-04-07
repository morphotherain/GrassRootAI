#include "UIShip.h"

using namespace DirectX;

bool UIShip::Init()
{
	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	return true;
}

void UIShip::OnResize()
{
}

void UIShip::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	m_tick = tick;
	UpdateLockedTargets(tick);
}

void UIShip::DrawUI()
{
	DrawLockedTarget();
	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();

	DirectX::XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);

	DrawCommonEffect(m_mainUnderlayEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_speedoUnderlayEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_gaugeRedEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_lowHighHeatEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_medHeatEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_plusMinusEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_capacitorEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_overloadBtnEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_slotBaseEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_slotOverloadBtnEffect, windowModel, viewMatrix, projMatrix);
	DrawCommonEffect(m_slotEquipmentEffect, windowModel, viewMatrix, projMatrix);
	DrawGaugeEffect(m_gaugeShieldEffect, windowModel, viewMatrix, projMatrix, 0.7f);
	DrawGaugeEffect(m_gaugeArmorEffect, windowModel, viewMatrix, projMatrix, 1.0f);
	DrawGaugeEffect(m_gaugeStructureEffect, windowModel, viewMatrix, projMatrix, 1.0f);
}

void UIShip::cleanup()
{
}

bool UIShip::InitResource()
{
	std::vector<PosTexIndex> verticesMainUnderlay = {};
	GenerateRectVertex(verticesMainUnderlay, 0.0f, 0.0f, 160.0f, 160.0f, 0.0f);
	InitCommonEffect(verticesMainUnderlay, { front + "mainUnderlay.dds" }, m_mainUnderlayEffect);

	std::vector<PosTexIndex> verticesSpeedoUnderlay = {};
	GenerateRectVertex(verticesSpeedoUnderlay, 29.0f, 105.0f, 104.0f, 44.0f, 0.0f);
	InitCommonEffect(verticesSpeedoUnderlay, { front + "speedoUnderlay.dds" }, m_speedoUnderlayEffect);

	std::vector<PosTexIndex> verticesGaugeRed = {};
	GenerateRectVertex(verticesGaugeRed, 6.0f, 6.0f, 148.0f, 148.0f, 0.0f);
	GenerateRectVertex(verticesGaugeRed, 6.0f, 6.0f, 148.0f, 148.0f, 1.0f);
	GenerateRectVertex(verticesGaugeRed, 6.0f, 6.0f, 148.0f, 148.0f, 2.0f);
	InitCommonEffect(verticesGaugeRed, {
		front + "gauge1Red.dds",
		front + "gauge2Red.dds",
		front + "gauge3Red.dds",
		}, m_gaugeRedEffect);

	std::vector<PosTexIndex> verticesGauge1 = {};
	GenerateRectVertex(verticesGauge1, 6.0f, 6.0f, 148.0f, 148.0f, 0.0f);
	InitGaugeEffect(verticesGauge1, { front + "gauge1.dds" }, m_gaugeShieldEffect);
	std::vector<PosTexIndex> verticesGauge2 = {};
	GenerateRectVertex(verticesGauge2, 6.0f, 6.0f, 148.0f, 148.0f, 0.0f);
	InitGaugeEffect(verticesGauge2, { front + "gauge2.dds" }, m_gaugeArmorEffect);
	std::vector<PosTexIndex> verticesGauge3 = {};
	GenerateRectVertex(verticesGauge3, 6.0f, 6.0f, 148.0f, 148.0f, 0.0f);
	InitGaugeEffect(verticesGauge3, { front + "gauge3.dds" }, m_gaugeStructureEffect);

	std::vector<PosTexIndex> verticesOverloadBtn = {};
	GenerateRectVertex(verticesOverloadBtn, 152.0f, 85.0f, 20.0f, 20.0f, 0.0f);
	GenerateRectVertex(verticesOverloadBtn, 162.0f, 70.0f, 20.0f, 20.0f, 1.0f);
	GenerateRectVertex(verticesOverloadBtn, 152.0f, 55.0f, 20.0f, 20.0f, 2.0f);
	InitCommonEffect(verticesOverloadBtn, {
		front + "overloadBtnLoOff.dds",
		front + "overloadBtnMedOff.dds",
		front + "overloadBtnHiOff.dds",
		front + "overloadBtnLoOn.dds",
		front + "overloadBtnMedOn.dds",
		front + "overloadBtnHiOn.dds",
		}, m_overloadBtnEffect);

	std::vector<PosTexIndex> verticesSlotOverloadBtn = {};
	GenerateSlotOverloadsVertices(verticesSlotOverloadBtn);
	InitCommonEffect(verticesSlotOverloadBtn, {
		front + "slotOverloadOn.dds",
		front + "slotOverloadOff.dds",
		front + "slotOverloadDisabled.dds"
		}, m_slotOverloadBtnEffect);

	std::vector<PosTexIndex> verticesSlotBase = {};
	GenerateSlotUnderlayVertices(verticesSlotBase);
	InitCommonEffect(verticesSlotBase, { front + "slotMainFull.dds" }, m_slotBaseEffect);

	InitEquipmentEffect(m_slotEquipmentEffect);

	std::vector<PosTexIndex> verticesLowHighHeat = {};
	GenerateRectVertex(verticesLowHighHeat, 38.0f, 42.0f, 28.0f, 40.0f, 0.0f);
	GenerateRectVertex(verticesLowHighHeat, 97.0f, 42.0f, 28.0f, 40.0f, 1.0f);
	InitCommonEffect(verticesLowHighHeat, {
		front + "lowHeat_0.dds",
		front + "hiHeat_0.dds",
		}, m_lowHighHeatEffect);

	std::vector<PosTexIndex> verticesMedHeat = {};
	GenerateRectVertex(verticesMedHeat, 59.0f, 36.0f, 48.0f, 20.0f, 0.0f);
	InitCommonEffect(verticesMedHeat, { front + "medHeat_0.dds" }, m_medHeatEffect);

	std::vector<PosTexIndex> verticesPlusMinus = {};
	GenerateRectVertex(verticesPlusMinus, 25.0f, 105.0f, 12.0f, 12.0f, 1.0f);
	GenerateRectVertex(verticesPlusMinus, 125.0f, 105.0f, 12.0f, 12.0f, 0.0f);
	InitCommonEffect(verticesPlusMinus, {
		front + "plus.dds",
		front + "minus.dds",
		}, m_plusMinusEffect);

	std::vector<PosTexIndex> verticesCapacitor = {};
	GenerateCapacitorDots(verticesCapacitor, 16, 80.0f, 80.0f, 25.0f);
	InitCommonEffect(verticesCapacitor, { front + "capacitorCell.dds" }, m_capacitorEffect);

	auto UIcamera = std::make_shared<OrthographicCamera>();
	m_pUICamera = UIcamera;

	// 设置正交投影参数
	float left = 0.0f;
	float right = static_cast<float>(m_ClientWidth);
	float bottom = static_cast<float>(m_ClientHeight);
	float top = 0.0f;
	float nearZ = 0.0f;
	float farZ = 1.0f;
	UIcamera->SetOrthographic(left, right, bottom, top, nearZ, farZ);

	return true;
}

bool UIShip::InitEffect()
{
	return true;
}

void UIShip::UpdateLockedTargets(UINT tick)
{
	for (auto& pair : m_mapLockedTarget) {
		pair.second->m_checked = false;
	}
	auto p_mapObject = SolarSystemMgr::getInstance().p_mapObject;
	auto m_currentShip = SolarSystemMgr::getInstance().currentPilot->currentShip;
	auto lockingComponent = m_currentShip->GetComponent<LockingComponent>();
	for (auto& pair : lockingComponent->m_mapLockedTarget) {
		int id = pair.second->m_TargetObjectID;
		auto it = m_mapLockedTarget.find(id);
		if (it == m_mapLockedTarget.end()) {
			GenerateLockedTarget(id);
		}
		m_mapLockedTarget[id]->m_checked = true;
		m_mapLockedTarget[id]->m_isLocked = pair.second->m_isLocked;
		m_mapLockedTarget[id]->m_lockedProcess = pair.second->m_lockedProcess;
	}
	std::vector<int> idsToRemove;
	for (auto& pair : m_mapLockedTarget) {
		if (!pair.second->m_checked) {
			idsToRemove.push_back(pair.first);
		}
	}

	for (int id : idsToRemove) {
		m_mapLockedTarget.erase(id);
	}

	auto Tran1 = m_currentShip->GetComponent<SpaceTransformComponent>();

	for (auto& pair : m_mapLockedTarget) {
		auto m_currentLockedTarget = (*p_mapObject)[pair.first];
		auto Tran2 = m_currentLockedTarget->GetComponent<SpaceTransformComponent>();
		auto ThirdCameraLocal = std::reinterpret_pointer_cast<ThirdPersonCamera>(m_pLocalCamera);
		auto sector = SolarSystemMgr::getInstance().currentSolarSystem->currentSector;
		DirectX::XMMATRIX viewMatrixLocal = m_pLocalCamera->GetViewXM();
		DirectX::XMMATRIX projMatrixLocal = m_pLocalCamera->GetProjXM();
		auto ndc = Convert3DToNDC({
			static_cast<float>(Tran2->x - sector->x),
			static_cast<float>(Tran2->y - sector->y),
			static_cast<float>(Tran2->z - sector->z) }, viewMatrixLocal, projMatrixLocal);
		auto pos = ConvertNDCToScreen(ndc, 1920.0f, 1080.0f);

		m_mapLockedTarget[pair.first]->target_x = pos.x + 10.0f;
		m_mapLockedTarget[pair.first]->target_y = pos.y + 16.0f;
	}
}

void UIShip::InitCommonEffect(std::vector<PosTexIndex>& _vertices, std::vector<std::string> textureFileName, std::shared_ptr<Effect>& m_effect)
{
	m_effect = std::make_shared<Effect>();
	m_effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_effect->getVertexBuffer<PosTexIndex>()->setVertices(_vertices);
	m_effect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_effect->addTextures(textureFileName);
	m_effect->addConstantBuffer<ConstantMVPIndex>();
	m_effect->Init();
}

void UIShip::DrawCommonEffect(std::shared_ptr<Effect>& m_effect, DirectX::XMMATRIX& windowModel, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projMatrix)
{
	ConstantMVPIndex* dataPtr = m_effect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(windowModel);
	dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_effect->apply();
}

void UIShip::InitEquipmentEffect(std::shared_ptr<Effect>& m_effect)
{
	int highTypeID = 483;
	int medTypeID = 439;
	int lowTypeID = 22542;
	std::vector<PosTexIndex> verticesSlotEquipment = {};
	GenerateSlotEquipmentVertices(verticesSlotEquipment, 0, 0, 0.0f);
	GenerateSlotEquipmentVertices(verticesSlotEquipment, 1, 0, 1.0f);
	GenerateSlotEquipmentVertices(verticesSlotEquipment, 2, 0, 2.0f);
	InitCommonEffect(verticesSlotEquipment, {
		getIconPathByTypeID(highTypeID),
		getIconPathByTypeID(medTypeID),
		getIconPathByTypeID(lowTypeID)
		}, m_slotEquipmentEffect);
}

void UIShip::InitGaugeEffect(std::vector<PosTexIndex>& _vertices, std::vector<std::string> textureFileName, std::shared_ptr<Effect>& m_effect)
{
	m_effect = std::make_shared<Effect>();
	m_effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\UI\\Gauge_VS.hlsl", L"HLSL\\UI\\Gauge_VS.cso");
	m_effect->getVertexBuffer<PosTexIndex>()->setVertices(_vertices);
	m_effect->addPixelShader(L"HLSL\\UI\\Gauge_PS.hlsl", L"HLSL\\UI\\Gauge_PS.cso");
	m_effect->addTextures(textureFileName);
	m_effect->addConstantBuffer<ConstantMVPIndex>();
	m_effect->Init();
}

void UIShip::DrawGaugeEffect(std::shared_ptr<Effect>& m_effect, DirectX::XMMATRIX& windowModel, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projMatrix, float hpRadio)
{
	ConstantMVPIndex* dataPtr = m_effect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(windowModel);
	dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = hpRadio;
	m_effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_effect->apply();
}

void UIShip::GenerateCapacitorDots(std::vector<PosTexIndex>& vertices, int numRadii, float centerX, float centerY, float circleRadius)
{
	// 计算每个半径之间的角度间隔
	float angleStep = 2 * DirectX::XM_PI / numRadii;

	// 遍历每个半径
	for (int i = 0; i < numRadii; ++i) {
		float angle = i * angleStep;

		// 每个半径上有 4 个小点
		for (int j = 0; j < 4; ++j) {
			// 计算小点的半径，从圆周到圆心逐渐缩小
			float dotRadius = circleRadius - j * (circleRadius / 4);

			// 计算小点的中心坐标
			float dotX = centerX + dotRadius * std::cos(angle);
			float dotY = centerY + dotRadius * std::sin(angle);

			// 计算小点的尺寸，从圆周到圆心逐渐缩小
			float dotSize = 10.0f - j * (8.0f / 4.0f);

			// 计算小点到圆心的向量
			float dx = centerX - dotX;
			float dy = centerY - dotY;

			// 计算小点的方向，始终朝向圆心
			float dotAngle = std::atan2(dy, dx) - DirectX::XM_PI / 2.0f;

			// 调用 GenerateRectVertex 函数生成矩形顶点
			//DEBUG_("GenerateRectVertex(vertices, dotX {}, dotY {}, dotSizeX {}, dotSizeY {}, dotAngle{}, 0.0f)", dotX, dotY, dotSize, dotSize, dotAngle);
			GenerateRectVertex(vertices, dotX - dotSize / 2.0f, dotY - dotSize / 2.0f, dotSize, dotSize, dotAngle, 0.0f);
		}
	}
}

void UIShip::GenerateSlotVertices(std::vector<PosTexIndex>& vertices, int row, int col, float texID)
{
	float delta = (row % 2 == 0) ? 0.0f : 30.0f;
	GenerateRectVertex(vertices, 155.0f + delta + col * 50.0f, 10.0f + row * 40.0f, 64.0f, 64.0f, texID);
}

void UIShip::GenerateSlotEquipmentVertices(std::vector<PosTexIndex>& vertices, int row, int col, float texID)
{
	float delta = (row % 2 == 0) ? 0.0f : 30.0f;
	GenerateRectVertex(vertices, 165.0f + delta + col * 50.0f, 18.0f + row * 40.0f, 48.0f, 48.0f, texID);
}

void UIShip::GenerateSlotUnderlayVertices(std::vector<PosTexIndex>& vertices)
{
	for (auto row = 0; row < 3; row++) {
		for (auto col = 0; col < 8; col++) {
			GenerateSlotVertices(vertices, row, col, 0.0f);
		}
	}
}

void UIShip::GenerateSlotOverloadVertices(std::vector<PosTexIndex>& vertices, int row, int col)
{
	float delta = (row % 2 == 0) ? 0.0f : 30.0f;
	GenerateRectVertex(vertices, 155.0f + 16.0f + delta + col * 50.0f, 15.0f + row * 40.0f, 32.0f, 16.0f, 1.0f);
}

void UIShip::GenerateSlotOverloadsVertices(std::vector<PosTexIndex>& vertices)
{
	for (auto row = 0; row < 3; row++) {
		for (auto col = 0; col < 8; col++) {
			GenerateSlotOverloadVertices(vertices, row, col);
		}
	}
}

void UIShip::GenerateLockedTarget(int objectID)
{
	auto p_mapObject = SolarSystemMgr::getInstance().p_mapObject;
	auto targetObject = (*p_mapObject)[objectID];
	auto base = targetObject->GetComponent<BaseComponent>();

	m_mapLockedTarget[objectID] = std::make_shared<LockedTarget>();
	auto& target = m_mapLockedTarget[objectID];
	target->x = 600.0f;
	target->y = 100.0f;
	target->target_x = 900.0f;
	target->target_y = 500.0f;
	target->m_lockedProcess = 0.4f;

	int typeID = base->typeID;

	std::vector<PosTexIndex> verticesSpeedoUnderlay = {};
	GenerateRectVertex(verticesSpeedoUnderlay, 25.0f, 25.0f, 50.0f, 50.0f, 0.0f);
	InitCommonEffect(verticesSpeedoUnderlay, {
		getIconPathByTypeID(typeID) }, target->m_typeImgEffect);

	std::vector<PosTexIndex> verticesButtonBody = {};
	GenerateRectVertex(verticesButtonBody, 20.0f, 65.0f, 60.0f, 28.0f, 0.0f);
	InitCommonEffect(verticesButtonBody, { front + "buttonBody.dds" }, target->m_bodyEffect);

	std::vector<PosTexIndex> verticesArrow = {};
	GenerateRectVertex(verticesArrow, 0.0f, 0.0f, 100.0f, 100.0f, 0.0f);
	InitCommonEffect(verticesArrow, { front + "releaseArrowOpacity.dds" }, target->m_arrowEffect);

	std::vector<PosTexIndex> verticesGaugeRed = {};
	GenerateRectVertex(verticesGaugeRed, 8.0f, 8.0f, 84.0f, 84.0f, 0.0f);
	GenerateRectVertex(verticesGaugeRed, 8.0f, 8.0f, 84.0f, 84.0f, 1.0f);
	GenerateRectVertex(verticesGaugeRed, 8.0f, 8.0f, 84.0f, 84.0f, 2.0f);
	InitCommonEffect(verticesGaugeRed, {
		front + "gauge1Red_target.dds",
		front + "gauge2Red_target.dds",
		front + "gauge3Red_target.dds",
		}, target->m_gaugeRedEffect);

	std::vector<PosTexIndex> verticesGauge1 = {};
	GenerateRectVertex(verticesGauge1, 8.0f, 8.0f, 84.0f, 84.0f, 0.0f);
	InitLockedTargetGaugeEffect(verticesGauge1, { front + "gauge1_target.dds" }, target->m_gaugeShieldEffect);
	std::vector<PosTexIndex> verticesGauge2 = {};
	GenerateRectVertex(verticesGauge2, 8.0f, 8.0f, 84.0f, 84.0f, 0.0f);
	InitLockedTargetGaugeEffect(verticesGauge2, { front + "gauge2_target.dds" }, target->m_gaugeArmorEffect);
	std::vector<PosTexIndex> verticesGauge3 = {};
	GenerateRectVertex(verticesGauge3, 8.0f, 8.0f, 84.0f, 84.0f, 0.0f);
	InitLockedTargetGaugeEffect(verticesGauge3, { front + "gauge3_target.dds" }, target->m_gaugeStructureEffect);

	std::vector<PosTexIndex> verticesLockedLine = {};
	GenerateRectVertex(verticesLockedLine, -10000.0f, 0.0f, 10000.0f - 42.0f, 1.0f, 0.0f);
	GenerateRectVertex(verticesLockedLine, 42.0f, 0.0f, 10000.0f - 42.0f, 1.0f, 0.0f);
	GenerateRectVertex(verticesLockedLine, 0.0f, -10000.0f, 1.0f, 10000.0f - 42.0f, 0.0f);
	GenerateRectVertex(verticesLockedLine, 0.0f, 42.0f, 1.0f, 10000.0f - 42.0f, 0.0f);
	InitCommonEffect(verticesLockedLine, { front + "locked_line.dds" }, target->m_lockedLineEffect);

	std::vector<PosTexIndex> verticesLockedCircle = {};
	GenerateRectVertex(verticesLockedCircle, -42.0f, -42.0f, 84.0f, 84.0f, 0.0f);
	InitCommonEffect(verticesLockedCircle, { front + "locked_circle.dds" }, target->m_lockedCircleEffect);

	std::vector<PosTexIndex> verticesLockingCircle = {};
	GenerateRectVertex(verticesLockingCircle, -42.0f, -42.0f, 84.0f, 84.0f, 0.0f);
	target->m_lockingCircleEffect = std::make_shared<Effect>();
	target->m_lockingCircleEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\UI\\ProcessCircle_VS.hlsl", L"HLSL\\UI\\ProcessCircle_VS.cso");
	target->m_lockingCircleEffect->getVertexBuffer<PosTexIndex>()->setVertices(verticesLockingCircle);
	target->m_lockingCircleEffect->addPixelShader(L"HLSL\\UI\\ProcessCircle_PS.hlsl", L"HLSL\\UI\\ProcessCircle_PS.cso");
	target->m_lockingCircleEffect->addTextures({ front + "locking_circle.dds" });
	target->m_lockingCircleEffect->addConstantBuffer<ConstantMVPIndex>();
	target->m_lockingCircleEffect->Init();

	target->m_lockedObjectName = std::make_shared<UIText>();
	target->m_lockedObjectName->setSize(30.0f, 100.0f, 350.0f, 350.0f);
	target->m_lockedObjectName->setText(InvTypesManager::getInstance()->getNameByTypeId(typeID));
	target->m_lockedObjectName->Init();
}

void UIShip::DeleteLockedTarget(int objectID)
{
}

void UIShip::InitLockedTargetGaugeEffect(std::vector<PosTexIndex>& _vertices, std::vector<std::string> textureFileName, std::shared_ptr<Effect>& m_effect)
{
	m_effect = std::make_shared<Effect>();
	m_effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\UI\\GaugeTarget_VS.hlsl", L"HLSL\\UI\\GaugeTarget_VS.cso");
	m_effect->getVertexBuffer<PosTexIndex>()->setVertices(_vertices);
	m_effect->addPixelShader(L"HLSL\\UI\\GaugeTarget_PS.hlsl", L"HLSL\\UI\\GaugeTarget_PS.cso");
	m_effect->addTextures(textureFileName);
	m_effect->addConstantBuffer<ConstantMVPIndex>();
	m_effect->Init();
}

void UIShip::DrawLockedTarget()
{
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();

	int index = 0;
	for (auto& pair : m_mapLockedTarget) {
		auto& target = pair.second;

		DirectX::XMMATRIX windowModelTarget = XMMatrixTranslation(target->target_x, target->target_y, 0.0f);
		DrawCommonEffect(target->m_lockedLineEffect, windowModelTarget, viewMatrix, projMatrix);
		DrawCommonEffect(target->m_lockedCircleEffect, windowModelTarget, viewMatrix, projMatrix);

		DirectX::XMMATRIX windowModel = XMMatrixTranslation(target->x + 120.0f * index, target->y, 0.0f);
		float angle = (XM_2PI) * (m_tick % 360) / 360.0f;
		if (!target->m_isLocked)
		{
			ConstantMVPIndex* dataPtr = target->m_lockingCircleEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
			dataPtr->model = XMMatrixTranspose(windowModelTarget);
			dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
			dataPtr->projection = XMMatrixTranspose(projMatrix);
			dataPtr->TexIndex = target->m_lockedProcess;
			target->m_lockingCircleEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
			target->m_lockingCircleEffect->apply();
		}
		else {
			target->m_lockedObjectName->setDelta(target->x + 120.0f * index, target->y);
			target->m_lockedObjectName->DrawUI();

			DrawCommonEffect(target->m_typeImgEffect, windowModel, viewMatrix, projMatrix);
			DrawCommonEffect(target->m_gaugeRedEffect, windowModel, viewMatrix, projMatrix);
			DrawGaugeEffect(target->m_gaugeShieldEffect, windowModel, viewMatrix, projMatrix, 0.7f);
			DrawGaugeEffect(target->m_gaugeArmorEffect, windowModel, viewMatrix, projMatrix, 1.0f);
			DrawGaugeEffect(target->m_gaugeStructureEffect, windowModel, viewMatrix, projMatrix, 1.0f);
			DrawCommonEffect(target->m_bodyEffect, windowModel, viewMatrix, projMatrix);

			windowModel = CreateRotatedWindowModel(target->x + 120.0f * index, target->y, 100.0f, 100.0f, angle);
			DrawCommonEffect(target->m_arrowEffect, windowModel, viewMatrix, projMatrix);
		}

		windowModel = CreateRotatedWindowModel(target->target_x - 50.0f, target->target_y - 50.0f, 100.0f, 100.0f, angle);
		DrawCommonEffect(target->m_arrowEffect, windowModel, viewMatrix, projMatrix);

		if (target->m_isLocked)
			index++;
	}
}