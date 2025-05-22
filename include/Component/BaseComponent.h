#pragma once

#include "Component.h"
#include "InvTypesManager.h"
#include "d3dUtil.h"

class BaseComponent : public Component {
public:
	BaseComponent() = default;
	BaseComponent(UINT objectID);
	~BaseComponent();

	std::vector<std::type_index> GetDependencies() const override {
		return {};
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override {}

	virtual void Update(UINT tick);
	virtual void OnDestroy();
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