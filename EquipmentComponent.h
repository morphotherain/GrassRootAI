#pragma once

#include "Component.h"
#include "AttributesComponent.h"
#include "StorageComponent.h"
#include "d3dUtil.h"

class EquipmentComponent : public Component {
public:
	EquipmentComponent() = default;
	EquipmentComponent(UINT _objectID);
	~EquipmentComponent() = default;

	std::vector<std::type_index> GetDependencies() const override {
		return {
			typeid(AttributesComponent)
		};
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override;
	void Switch (int targetObjectId);

	virtual void Update(UINT tick);
	virtual void handleTask(const Task& task);

	std::shared_ptr<AttributesComponent> m_pAttributes;
	float m_activateProcess = 0.0f;
	float m_totalActivateTime = 6.0f;
	bool keep_active = false;

	bool m_beginActive = false;
	bool m_endActive = false;
	bool m_isActiving = false;

	int m_targetObjectId = -1;

	UINT objectID;
	std::wstring name;
};