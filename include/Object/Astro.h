#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "WarpGateComponent.h"
#include <functional>
#include <string>
#include <unordered_map>

struct Task;

class Star : public GameObject
{
public:
	Star() = default;
	Star(UINT _objectID) :objectID(_objectID) {};
	~Star() = default;
	virtual void Init() {}
	UINT objectID;
};

class Planet : public GameObject
{
public:
	Planet() = default;
	Planet(UINT _objectID) :objectID(_objectID) {};
	~Planet() = default;
	virtual void Init() {}
	UINT objectID;
};

class Moon : public GameObject
{
public:
	Moon() = default;
	Moon(UINT _objectID) :objectID(_objectID) {};
	~Moon() = default;
	virtual void Init() {}
	UINT objectID;
};

class AsteroidBelt : public GameObject
{
public:
	AsteroidBelt() = default;
	AsteroidBelt(UINT _objectID) :objectID(_objectID) {};
	~AsteroidBelt() = default;
	virtual void Init() {}
	UINT objectID;
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
	void fillObjectName();

	virtual void handleTask(const Task& task);
	void initTaskHandlers();
	std::unordered_map<std::string, std::function<void(const Task&)>> taskHandlers;
};

class Astro : public GameObject
{
public:
	Astro() = default;
	Astro(UINT _objectID) :objectID(_objectID) {};
	~Astro() = default;

	virtual void Init() {};
	virtual std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID);

	UINT objectID;
};
