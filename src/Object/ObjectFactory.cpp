#include "ObjectFactory.h"
#include "Astro.h"
#include "NPCStation.h"
#include "Material.h"
#include "Ship.h"
#include "Equipment.h"
#include "Asteroid.h"

std::shared_ptr<GameObject> ObjectFactory::CreateFromDynObject(const dynGameObject& objectData)
{
	std::shared_ptr<GameObject> object;
	switch (objectData.categoryID)
	{
	case 2:
		object = std::make_shared<Astro>(objectData.objectID);
		break;
	case 3:
		object = std::make_shared<NPCStation>(objectData.objectID);
		break;
	case 4:
		object = std::make_shared<Material>(objectData.objectID);
		break;
	case 5:
		return nullptr;
	case 6:
		object = std::make_shared<Ship>(objectData.objectID);
		break;
	case 7:
		object = std::make_shared<Equipment>(objectData.objectID);
		break;
	case 25:
		object = std::make_shared<Asteroid>(objectData.objectID);
		break;
	default:
		return nullptr;
	}

	if (auto refined = object->ConvertBasedOnGroupID(objectData.groupID))
	{
		object = refined;
	}

	object->Init();
	return object;
}
