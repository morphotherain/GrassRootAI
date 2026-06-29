#pragma once

#include "Component.h"

class WarpGateComponent : public Component {
public:
	WarpGateComponent() = default;
	WarpGateComponent(UINT objectID);
	~WarpGateComponent() = default;

	std::vector<std::type_index> GetDependencies() const override {
		return {};
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override {}

	virtual void Update(UINT tick) {};

	UINT objectID;
	std::wstring name;
};