#pragma once

#include <memory>
#include <vector>

class GameObject;

class EntityUpdateSystem {
public:
	void Bind(std::vector<std::weak_ptr<GameObject>>* spaceObjects);
	void Update(unsigned int tick);

private:
	std::vector<std::weak_ptr<GameObject>>* m_spaceObjects = nullptr;
};
