#include "Material.h"

std::shared_ptr<GameObject> Material::ConvertBasedOnGroupID(UINT groupID)
{
	switch (groupID) {
	case 18: {
		return std::make_shared<Mineral>(objectID);
	}
	}
	return nullptr;
}


void Mineral::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pAttributes = std::make_shared<AttributesComponent>(objectID);
	AddComponent<Component>(m_pAttributes);
}

void Mineral::Update(UINT tick) {

}