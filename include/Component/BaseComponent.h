#pragma once

#include "Component.h"
#include "InvTypesManager.h"
#include "Sim/AssetLocation.h"

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
	void MarkDirty() { needStore = true; }
	void setSolarSystemID(UINT solarSystemID);

	AssetOwner GetAssetOwner() const;
	AssetLocation GetAssetLocation() const;
	void SetAssetOwner(const AssetOwner& owner);
	void SetAssetLocation(const AssetLocation& location);
	bool needStore = false;

	UINT objectID = 0;
	UINT typeID = 0;
	UINT solarSystemID = 0;
	UINT ownerKind = 0;
	UINT ownerID = 0;
	UINT locationKind = 0;
	UINT locationRef = 0;
	UINT groupID = 0;
	UINT categoryID = 0;
	std::wstring name;
};
