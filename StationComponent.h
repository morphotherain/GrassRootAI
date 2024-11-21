#pragma once

#include "Component.h"
#include "d3dUtil.h"

class StationComponent : public Component {
public:
	StationComponent() = default;
	StationComponent(UINT objectID);
	~StationComponent() = default;

	virtual void Update(UINT tick) {};

	UINT objectID;
	std::wstring name;


};