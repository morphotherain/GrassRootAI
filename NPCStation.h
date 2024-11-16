#include "GameObject.h"
#include "BaseComponent.h"
#include "PhysicsComponent.h"
#include "SpaceTransformComponent.h"

class NPCStation : public GameObject {
public:
	NPCStation() = default;
	NPCStation(UINT _objectID) :objectID(_objectID) {};
	~NPCStation() = default;

	virtual void Init();

	UINT objectID;

	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;

};