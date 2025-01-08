#pragma once

#include "Component.h"
#include "d3dUtil.h"

class StorageComponent : public Component {
public:
	StorageComponent() = default;
	StorageComponent(UINT _objectID);
	~StorageComponent() = default;

	virtual void Update(UINT tick) {};

	UINT objectID;
	std::wstring name;


};