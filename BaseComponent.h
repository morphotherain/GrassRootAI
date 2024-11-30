#pragma once

#include "Component.h"
#include "InvTypesManager.h"
#include "d3dUtil.h"

class BaseComponent : public Component {
public:
	BaseComponent() = default;
	BaseComponent(UINT objectID);
	~BaseComponent() = default;

	virtual void Update(UINT tick) {};

	UINT objectID;
	UINT typeID;
	UINT solarSystemID;
	UINT ownerID;
	UINT containerID;
	UINT groupID;
	UINT categoryID;
	std::wstring name;


};