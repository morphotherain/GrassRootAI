#pragma once

#include "Component.h"
#include "InvTypesManager.h"
#include "d3dUtil.h"

class BaseComponent : public Component {
public:
	BaseComponent() = default;
	BaseComponent(UINT objectID);
	~BaseComponent();

	virtual void Update(UINT tick);
	void store();
	void setSolarSystemID(UINT solarSystemID);
	void setContainerID(UINT containerID);
	bool needStore = false;

	UINT objectID;
	UINT typeID;
	UINT solarSystemID;
	UINT ownerID;
	UINT containerID;
	UINT groupID;
	UINT categoryID;
	std::wstring name;


};