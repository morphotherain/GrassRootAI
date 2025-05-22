#include "WeaponComponent.h"
#include "BaseComponent.h"
#include "GameObject.h"

WeaponComponent::WeaponComponent(UINT _objectID)
{
	objectID = _objectID;
}

void WeaponComponent::InjectDependency(const std::shared_ptr<Component>& dep)
{
	if (auto pComponent = std::dynamic_pointer_cast<AttributesComponent>(dep)) {
		m_pAttributes = pComponent;
	}
	if (auto pComponent = std::dynamic_pointer_cast<EquipmentComponent>(dep)) {
		m_pEquipment = pComponent;
	}

}

void WeaponComponent::Update(UINT tick)
{

}

void WeaponComponent::handleTask(const Task& task)
{
	try {

	}
	catch (const std::bad_any_cast& e) {
		// 记录日志或进行错误处理
		DEBUG_("类型转换错误: {}", e.what());
	}
	DEBUG_(task.to_string());
}

void WeaponComponent::applyDamage()
{
	auto calculatorEM = AttributeCalculatorManager::getInstance().getCalculator(CALCULATOR_EM_DAMAGE);
	auto calculatorEX = AttributeCalculatorManager::getInstance().getCalculator(CALCULATOR_EXPLOSIVE_DAMAGE);
	auto calculatorKI = AttributeCalculatorManager::getInstance().getCalculator(CALCULATOR_KINETIC_DAMAGE);
	auto calculatorTH = AttributeCalculatorManager::getInstance().getCalculator(CALCULATOR_THERMAL_DAMAGE);
	double em_damage = 0.0f;
	double ex_damage = 0.0f;
	double ki_damage = 0.0f;
	double th_damage = 0.0f;
	if (calculatorEM) {
		em_damage = calculatorEM->calculate(*m_pAttributes->objectAttributes);
	}
	if (calculatorEX) {
		ex_damage = calculatorEX->calculate(*m_pAttributes->objectAttributes);
	}
	if (calculatorKI) {
		ki_damage = calculatorKI->calculate(*m_pAttributes->objectAttributes);
	}
	if (calculatorTH) {
		th_damage = calculatorTH->calculate(*m_pAttributes->objectAttributes);
	}
	auto target = GameObjectMgr::getInstance().getObject(m_pEquipment->m_targetObjectId);
	if (target) {
		auto attributeComponent = target->GetComponent<AttributesComponent>();
		double totalDamage = 0.0f;
		totalDamage += em_damage * (*attributeComponent->objectAttributes)[ATTR_ID_SHIELD_EM_DAMAGE_RESONANCE].value;
		totalDamage += ex_damage * (*attributeComponent->objectAttributes)[ATTR_ID_SHIELD_EXPLOSIVE_DAMAGE_RESONANCE].value;
		totalDamage += ki_damage * (*attributeComponent->objectAttributes)[ATTR_ID_SHIELD_KINETIC_DAMAGE_RESONANCE].value;
		totalDamage += th_damage * (*attributeComponent->objectAttributes)[ATTR_ID_SHIELD_THERMAL_DAMAGE_RESONANCE].value;

		attributeComponent->setAttrValueById(ATTR_ID_CURRENT_SHIELD, (*attributeComponent->objectAttributes)[5002].value - totalDamage);

		EventLogManager::getInstance().createEventLog(1, 2);
		std::unordered_map<std::wstring, std::any> content1 = { {L"伤害", totalDamage}, {L"武器名称", std::wstring(L"民用加特林脉冲激光炮")} };
		EventLogManager::getInstance().addContentToLatestLog(content1);
	}
}
