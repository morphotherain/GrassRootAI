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

	eventLogMgr.update(tick);

	auto m_currentShip = SolarSystemMgr::getInstance().currentPilot->currentShip;
	auto lockingComponent = m_currentShip->GetComponent<LockingComponent>();
	int currentSelectLockingObjectId;
	if (lockingComponent->m_mapLockedTarget.empty())
		currentSelectLockingObjectId = -1;
	else {
		currentSelectLockingObjectId = lockingComponent->m_mapLockedTarget.begin()->second->m_TargetObjectID;
	}

	std::shared_ptr<Task> task = std::make_shared<Task>();
	task->isInnerTask = true;
	task->taskID = -1;
	task->publisher = m_currentShip;
	task->target = m_currentShip;
	task->taskTypeId = 5;
	(*task->paramsPtr)["targetObjectID"] = currentSelectLockingObjectId;
	(*task->paramsPtr)["taskType"] = std::string("switch");
	while (true) {
		(*task->paramsPtr)["slotType"] = std::string("low");
		if (keyState.LeftControl && keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F1)){
			(*task->paramsPtr)["slotIndex"] = 0; break;
		}if (keyState.LeftControl && keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F2)) {
			(*task->paramsPtr)["slotIndex"] = 1; break;
		}if (keyState.LeftControl && keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F3)) {
			(*task->paramsPtr)["slotIndex"] = 2; break;
		}if (keyState.LeftControl && keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F4)) {
			(*task->paramsPtr)["slotIndex"] = 3; break;
		}if (keyState.LeftControl && keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F5)) {
			(*task->paramsPtr)["slotIndex"] = 4; break;
		}if (keyState.LeftControl && keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F6)) {
			(*task->paramsPtr)["slotIndex"] = 5; break;
		}if (keyState.LeftControl && keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F7)) {
			(*task->paramsPtr)["slotIndex"] = 6; break;
		}if (keyState.LeftControl && keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F8)) {
			(*task->paramsPtr)["slotIndex"] = 7; break;
		}

		(*task->paramsPtr)["slotType"] = std::string("medium");
		if (keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F1)) {
			(*task->paramsPtr)["slotIndex"] = 0; break;
		}if (keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F2)) {
			(*task->paramsPtr)["slotIndex"] = 1; break;
		}if (keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F3)) {
			(*task->paramsPtr)["slotIndex"] = 2; break;
		}if (keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F4)) {
			(*task->paramsPtr)["slotIndex"] = 3; break;
		}if (keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F5)) {
			(*task->paramsPtr)["slotIndex"] = 4; break;
		}if (keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F6)) {
			(*task->paramsPtr)["slotIndex"] = 5; break;
		}if (keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F7)) {
			(*task->paramsPtr)["slotIndex"] = 6; break;
		}if (keyState.LeftShift && m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F8)) {
			(*task->paramsPtr)["slotIndex"] = 7; break;
		}
		(*task->paramsPtr)["slotType"] = std::string("high");
		if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F1)) {
			(*task->paramsPtr)["slotIndex"] = 0; break;
		}if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F2)) {
			(*task->paramsPtr)["slotIndex"] = 1; break;
		}if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F3)) {
			(*task->paramsPtr)["slotIndex"] = 2; break;
		}if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F4)) {
			(*task->paramsPtr)["slotIndex"] = 3; break;
		}if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F5)) {
			(*task->paramsPtr)["slotIndex"] = 4; break;
		}if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F6)) {
			(*task->paramsPtr)["slotIndex"] = 5; break;
		}if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F7)) {
			(*task->paramsPtr)["slotIndex"] = 6; break;
		}if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::F8)) {
			(*task->paramsPtr)["slotIndex"] = 7; break;
		}
		
		break;
	}
	if(task->paramsPtr->find("slotIndex") != task->paramsPtr->end())
		TaskMgr::getInstance().addTask(task);

	std::shared_ptr<Task> taskSwitchLockingTarget = std::make_shared<Task>();
	taskSwitchLockingTarget->isInnerTask = true;
	taskSwitchLockingTarget->taskID = -1;
	taskSwitchLockingTarget->publisher = m_currentShip;
	taskSwitchLockingTarget->target = m_currentShip;
	taskSwitchLockingTarget->taskTypeId = 6;
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::OemComma)) {
		(*taskSwitchLockingTarget->paramsPtr)["direction"] = 0;
		TaskMgr::getInstance().addTask(taskSwitchLockingTarget);
	}
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::OemPeriod)) {
		(*taskSwitchLockingTarget->paramsPtr)["direction"] = 1;
		TaskMgr::getInstance().addTask(taskSwitchLockingTarget);
	}

	m_tick = tick;
	UpdateLockedTargets(tick);
}

void UIShip::DrawUI()
{
	eventLogMgr.draw();
	DrawLockedTarget();
	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();

	DirectX::XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);

	auto m_currentShip = SolarSystemMgr::getInstance().currentPilot->currentShip;
	auto attrComponent = m_currentShip->GetComponent<AttributesComponent>();
	auto& attributes = (*attrComponent->objectAttributes);
	auto calculator1 = AttributeCalculatorManager::getInstance().getCalculator(CALCULATOR_SHIELD_HP_MAX);
	double shield_hp_max = 0.0f;
	if (calculator1) {
		shield_hp_max = calculator1->calculate(*attrComponent->objectAttributes);
	}

	float shieldRadio = attributes[ATTR_ID_CURRENT_SHIELD].value / shield_hp_max;
	float armorRadio = attributes[ATTR_ID_CURRENT_ARMOR].value / attributes[ATTR_ID_ARMOR_HP].value;
	float structureRadio = attributes[ATTR_ID_CURRENT_HP].value / attributes[ATTR_ID_STRUCTURE_VALUE].value;

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
	DrawCommonEffect(m_slotEquipmentsEffect, windowModel, viewMatrix, projMatrix);
	DrawSlotRamp(windowModel, viewMatrix, projMatrix);
	DrawGaugeEffect(m_gaugeShieldEffect, windowModel, viewMatrix, projMatrix, shieldRadio);
	DrawGaugeEffect(m_gaugeArmorEffect, windowModel, viewMatrix, projMatrix, armorRadio);
	DrawGaugeEffect(m_gaugeStructureEffect, windowModel, viewMatrix, projMatrix, structureRadio);
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

	InitEquipmentsEffect(m_slotEquipmentsEffect);

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
	m_currentLockingTargetId = lockingComponent->currentLockedTargetId;

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
		auto pos = ConvertNDCToScreen(ndc, 1920, 1080);

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

void UIShip::InitEquipmentsEffect(std::shared_ptr<Effect>& m_effect)
{
	auto m_currentShip = SolarSystemMgr::getInstance().currentPilot->currentShip;
	auto equipmentComponent = m_currentShip->GetComponent<EquipmentsComponent>();
	std::vector<std::vector<int>> ItemsIDs = {};
	ItemsIDs.push_back(equipmentComponent->m_pHighSlot->itemIDs);
	ItemsIDs.push_back(equipmentComponent->m_pMediumSlot->itemIDs);
	ItemsIDs.push_back(equipmentComponent->m_pLowSlot->itemIDs);
	
	int slotIndex = 0;
	float textureIndex = 0.0f;
	std::vector<std::string> texturesArray = {};
	std::vector<PosTexIndex> verticesSlotEquipments = {};
	for (auto& ids : ItemsIDs) {
		float index = 0.0f;
		for (auto id : ids) {
			DEBUG_("load equipment:{}", id);
			auto targetObject = SolarSystemMgr::getInstance().getObjectById(id);
			auto typeID = targetObject->GetComponent<BaseComponent>()->typeID;
			texturesArray.push_back(getIconPathByTypeID(typeID));
			GenerateSlotEquipmentsVertices(verticesSlotEquipments, slotIndex, index, textureIndex);

			{
				std::vector<PosTexIndex> verticesSlotRamp = {};
				m_mapSlotRampEffect[id] = std::make_shared<Effect>();
				GenerateSlotVertices(verticesSlotRamp, slotIndex, index, 0.0f);
				m_mapSlotRampEffect[id]->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\UI\\ProcessCircle_VS.hlsl", L"HLSL\\UI\\ProcessCircle_VS.cso");
				m_mapSlotRampEffect[id]->getVertexBuffer<PosTexIndex>()->setVertices(verticesSlotRamp);
				m_mapSlotRampEffect[id]->addPixelShader(L"HLSL\\UI\\ProcessCircle_PS.hlsl", L"HLSL\\UI\\ProcessCircle_PS.cso");
				m_mapSlotRampEffect[id]->addTextures({ front + "slotRamp.dds" });
				m_mapSlotRampEffect[id]->addConstantBuffer<ConstantMVPIndex>();
				m_mapSlotRampEffect[id]->Init();
			}

			index++; textureIndex++;
		}
		slotIndex++;
	}
	InitCommonEffect(verticesSlotEquipments, texturesArray, m_effect);

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
	GenerateRectVertex(vertices, 155.0f + delta + col * 55.0f, 10.0f + row * 45.0f, 64.0f, 64.0f, texID);
}

void UIShip::GenerateSlotEquipmentsVertices(std::vector<PosTexIndex>& vertices, int row, int col, float texID)
{
	float delta = (row % 2 == 0) ? 0.0f : 30.0f;
	GenerateRectVertex(vertices, 165.0f + delta + col * 55.0f, 18.0f + row * 45.0f, 48.0f, 48.0f, texID);
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
	GenerateRectVertex(vertices, 155.0f + 16.0f + delta + col * 55.0f, 15.0f + row * 45.0f, 32.0f, 16.0f, 1.0f);
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
		auto object = SolarSystemMgr::getInstance().getObjectById(pair.first);
		float shield_hp_radio = 0.0f;
		float armor_hp_radio = 0.0f;
		float structure_hp_radio = 0.0f;
		if (object) {
			if (object->GetComponent<AttributesComponent>())
			{
				auto attr = object->GetComponent<AttributesComponent>();
				shield_hp_radio = (*attr->objectAttributes)[ATTR_ID_CURRENT_SHIELD].value / (*attr->objectAttributes)[ATTR_ID_SHIELD_CAPACITY].value;
				armor_hp_radio = (*attr->objectAttributes)[ATTR_ID_CURRENT_ARMOR].value / (*attr->objectAttributes)[ATTR_ID_ARMOR_HP].value;
				structure_hp_radio = (*attr->objectAttributes)[ATTR_ID_CURRENT_HP].value / (*attr->objectAttributes)[ATTR_ID_STRUCTURE_VALUE].value;

			}
		}

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
			DrawGaugeEffect(target->m_gaugeShieldEffect, windowModel, viewMatrix, projMatrix, shield_hp_radio);
			DrawGaugeEffect(target->m_gaugeArmorEffect, windowModel, viewMatrix, projMatrix, armor_hp_radio);
			DrawGaugeEffect(target->m_gaugeStructureEffect, windowModel, viewMatrix, projMatrix, structure_hp_radio);
			DrawCommonEffect(target->m_bodyEffect, windowModel, viewMatrix, projMatrix);

			if (m_currentLockingTargetId == pair.first){
				windowModel = CreateRotatedWindowModel(target->x + 120.0f * index, target->y, 100.0f, 100.0f, angle);
				DrawCommonEffect(target->m_arrowEffect, windowModel, viewMatrix, projMatrix);
			}
		}

		if (m_currentLockingTargetId == pair.first) {
			windowModel = CreateRotatedWindowModel(target->target_x - 50.0f, target->target_y - 50.0f, 100.0f, 100.0f, angle);
			DrawCommonEffect(target->m_arrowEffect, windowModel, viewMatrix, projMatrix);
		}

		if (target->m_isLocked)
			index++;
	}
}

void UIShip::DrawSlotRamp(DirectX::XMMATRIX& windowModel, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projMatrix)
{
	auto m_currentShip = SolarSystemMgr::getInstance().currentPilot->currentShip;
	auto equipmentComponent = m_currentShip->GetComponent<EquipmentsComponent>();
	std::vector<std::vector<int>> ItemsIDs = {};
	ItemsIDs.push_back(equipmentComponent->m_pHighSlot->itemIDs);
	ItemsIDs.push_back(equipmentComponent->m_pMediumSlot->itemIDs);
	ItemsIDs.push_back(equipmentComponent->m_pLowSlot->itemIDs);

	for (auto& ids : ItemsIDs) {
		for (auto id : ids) {
			auto m_slotRampEffect = m_mapSlotRampEffect[id];
			auto object = SolarSystemMgr::getInstance().getObjectById(id);
			auto equipmentComponent = object->GetComponent<EquipmentComponent>();
			DrawGaugeEffect(m_slotRampEffect, windowModel, viewMatrix, projMatrix, equipmentComponent->m_activateProcess);
		}
	}
}

void UIShip::EventLogMgr::update(UINT tick)
{
	// 每60帧生成一个测试日志
	if (tick % 10 == 0) {
		createTestLog();
	}

	// 更新所有日志的透明度
	for (auto it = logs.begin(); it != logs.end();) {
		if (it->isFadingIn) {
			it->transparency = (std::min)(1.0f, it->transparency + it->fadeInRate);
			if (it->transparency >= 1.0f) {
				it->isFadingIn = false;
			}
		}
		else {
			if (it->transparency > 0.8f)
				it->transparency = (std::max)(0.0f, it->transparency - it->fadeRate * 0.8f);
			else
				it->transparency = (std::max)(0.0f, it->transparency - it->fadeRate * 2.0f);
			if (it->transparency <= 0.0f) {
				it = logs.erase(it);
				continue;
			}
		}
		it->richText->setTransparency(it->transparency);
		++it;
	}

	// 调整日志位置，实现滚动效果
	int count = logs.size();
	for (auto& log : logs) {
		int delta = index - log.index;
		float y = 300.0f - delta * 20.0f;
		float deltaY = log.richText->y - y;
		int distance = deltaY / 20.0f;
		if (deltaY > 1.0f)
			log.richText->y -= 1.0f;
		if (--count > 4) {
			log.fadeRate = 0.01f;
		}
	}
}

void UIShip::EventLogMgr::createTestLog()
{

	auto log = EventLogManager::getInstance().popEventLog();
	if (log)
	{
		auto richText = std::make_shared<UIRichText>();
		richText->setSize(850.0f, 300.0f, 200.0f, 40.0f);

		int damage = static_cast<int>(log->getParamOrDefault<double>(L"伤害", 0.0f));
		std::wstring weaponName = log->getParamOrDefault<std::wstring>(L"武器名称", L"");

		richText->addTextSegment(std::to_wstring(damage) + L" ", D2D1::ColorF(D2D1::ColorF::CadetBlue), 13.0f);
		richText->addTextSegment(L" 对 ", D2D1::ColorF(D2D1::ColorF::Gray), 11.0f);
		richText->addTextSegment(L" 狞獾级", D2D1::ColorF(D2D1::ColorF::White), 13.0f);
		richText->addTextSegment(L" - 穿透", D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.6f), 11.0f);
		richText->addTextSegment(std::wstring(L" ") + std::wstring(weaponName.begin(), weaponName.end()), D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.4f), 11.0f);
		richText->setTextCentered(true);
		richText->Init();

		LogEntry entry;
		entry.richText = richText;
		entry.transparency = 0.0f;
		entry.fadeRate = 0.003f;
		entry.fadeInRate = 0.1f;
		entry.isFadingIn = true;
		entry.index = index;
		index++;

		logs.push_back(entry);
	}

}