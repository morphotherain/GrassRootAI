#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

class Component;

class ComponentDependencyBinder {
public:
	template<typename T>
	void Bind(std::function<void(T*)> setter)
	{
		bindings_[std::type_index(typeid(T))] = [setter = std::move(setter)](const std::shared_ptr<Component>& dep) {
			if (auto* typed = dynamic_cast<T*>(dep.get())) {
				setter(typed);
			}
		};
	}

	template<typename T>
	void BindShared(std::function<void(const std::shared_ptr<T>&)> setter)
	{
		bindings_[std::type_index(typeid(T))] = [setter = std::move(setter)](const std::shared_ptr<Component>& dep) {
			if (auto typed = std::dynamic_pointer_cast<T>(dep)) {
				setter(typed);
			}
		};
	}

	void Inject(const std::shared_ptr<Component>& dep)
	{
		const auto it = bindings_.find(std::type_index(typeid(*dep)));
		if (it != bindings_.end()) {
			it->second(dep);
		}
	}

private:
	std::unordered_map<std::type_index, std::function<void(const std::shared_ptr<Component>&)>> bindings_;
};
