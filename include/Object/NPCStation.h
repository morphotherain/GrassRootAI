#include "GameObject.h"
#include <functional>
#include <string>
#include <unordered_map>

class Task;

class NPCStation : public GameObject {
public:
	NPCStation() = default;
	NPCStation(UINT _objectID) :objectID(_objectID) {};
	~NPCStation() = default;

	virtual void Init();
	virtual void Update(UINT tick);

	UINT objectID;

	void fillObjectName();

	virtual void handleTask(const Task& task);
	std::unordered_map<std::string, std::function<void(const Task&)>> taskHandlers;
	void initTaskHandlers();
};
