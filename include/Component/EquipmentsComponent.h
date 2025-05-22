#pragma once

#include "Component.h"
#include "GameObject.h"
#include "AttributesComponent.h"
#include "LockingComponent.h"
#include "StorageComponent.h"
#include "d3dUtil.h"

class EquipmentsComponent : public Component {
public:
	EquipmentsComponent() = default;
	EquipmentsComponent(UINT _objectID);
	~EquipmentsComponent() = default;

	std::vector<std::type_index> GetDependencies() const override {
		return {
			typeid(AttributesComponent),
			typeid(LockingComponent),
			typeid(HighSlotComponent),
			typeid(MediumSlotComponent),
			typeid(LowSlotComponent),
			typeid(RigSlotComponent),
		};
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override;

	virtual void Update(UINT tick);

	virtual void handleTask(const Task& task);


	std::shared_ptr<LockingComponent> m_pLocking;
	std::shared_ptr<AttributesComponent> m_pAttributes;
	std::shared_ptr<HighSlotComponent> m_pHighSlot;
	std::shared_ptr<MediumSlotComponent> m_pMediumSlot;
	std::shared_ptr<LowSlotComponent> m_pLowSlot;
	std::shared_ptr<RigSlotComponent> m_pRigSlot;

	UINT objectID;
	std::wstring name;
};