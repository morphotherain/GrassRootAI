#pragma once

#include "Component.h"
#include "AttributesComponent.h"
#include "EquipmentComponent.h"
#include "StorageComponent.h"
#include "d3dUtil.h"

class WeaponComponent : public Component {
public:
	WeaponComponent() = default;
	WeaponComponent(UINT _objectID);
	~WeaponComponent() = default;

	std::vector<std::type_index> GetDependencies() const override {
		return {
			typeid(AttributesComponent),
			typeid(EquipmentComponent),
		};
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override;

	virtual void Update(UINT tick);
	virtual void handleTask(const Task& task);

	void applyDamage();

	std::shared_ptr<AttributesComponent> m_pAttributes;
	std::shared_ptr<EquipmentComponent> m_pEquipment;

	UINT objectID;
	std::wstring name;
};