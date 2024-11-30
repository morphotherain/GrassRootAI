#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"

class Star : public GameObject
{
public:
	Star() = default;
	Star(UINT _objectID) :objectID(_objectID) {};
	~Star() = default;

	virtual void Init();

	UINT objectID;
	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;

};


class Planet : public GameObject
{
public:
	Planet() = default;
	Planet(UINT _objectID) :objectID(_objectID) {};
	~Planet() = default;

	virtual void Init();

	UINT objectID;
	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;

};

class Moon : public GameObject
{
public:
	Moon() = default;
	Moon(UINT _objectID) :objectID(_objectID) {};
	~Moon() = default;

	virtual void Init();

	UINT objectID;
	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;

};

class AsteroidBelt : public GameObject
{
public:
	AsteroidBelt() = default;
	AsteroidBelt(UINT _objectID) :objectID(_objectID) {};
	~AsteroidBelt() = default;

	virtual void Init();

	UINT objectID;
	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;

};

class StarGate : public GameObject
{
public:
	StarGate() = default;
	StarGate(UINT _objectID) :objectID(_objectID) {};
	~StarGate() = default;

	virtual void Init();
	virtual void Update(UINT tick);

	UINT objectID;
	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;
	std::shared_ptr<WarpGateComponent>m_pWarpGate;
	void fillObjectName();


	virtual void handleTask(const Task& task);

};


class Astro : public GameObject
{
public:
	Astro() = default;
	Astro(UINT _objectID) :objectID(_objectID){};
	~Astro() = default;

	virtual void Init() {};
	virtual std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID);

	UINT objectID;

	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;

};

