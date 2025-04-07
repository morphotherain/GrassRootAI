#pragma once

#include "Component.h"
#include "d3dUtil.h"

class WarpGateComponent : public Component {
public:
	WarpGateComponent() = default;
	WarpGateComponent(UINT objectID);
	~WarpGateComponent() = default;

	virtual void Update(UINT tick) {};

	UINT objectID;
	std::wstring name;
};