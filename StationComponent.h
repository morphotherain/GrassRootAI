#pragma once

#include "Component.h"
#include "d3dUtil.h"

class StationComponent : public Component {
public:
	StationComponent() = default;
	StationComponent(UINT objectID);
	~StationComponent() = default;

	std::vector<std::type_index> GetDependencies() const override {
		return {};
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override {}

	virtual void Update(UINT tick) {};

	UINT objectID;
	std::wstring name;
};