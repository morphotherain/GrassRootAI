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
	m_pBase = GetComponentShared<BaseComponent>();
	m_pAttributes = GetComponentShared<AttributesComponent>();
}

void Mineral::Update(UINT tick) {

}