#include "EntityUpdateSystem.h"

#include "GameObject.h"

void EntityUpdateSystem::Bind(std::vector<std::weak_ptr<GameObject>>* spaceObjects)
{
	m_spaceObjects = spaceObjects;
}

void EntityUpdateSystem::Update(unsigned int tick)
{
	if (!m_spaceObjects) {
		return;
	}

	auto it = std::remove_if(m_spaceObjects->begin(), m_spaceObjects->end(),
		[](const std::weak_ptr<GameObject>& wp) {
			return wp.expired();
		});
	m_spaceObjects->erase(it, m_spaceObjects->end());

	for (auto obj : *m_spaceObjects) {
		if (auto p = obj.lock()) {
			p->Update(tick);
		}
	}
}
