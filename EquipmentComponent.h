#pragma once

#include "Component.h"
#include "d3dUtil.h"

class EquipmentComponent : public Component {
public:
	EquipmentComponent() = default;
	EquipmentComponent(UINT _objectID);
	~EquipmentComponent() = default;

	virtual void Update(UINT tick) {};

	UINT objectID;
	std::wstring name;
};