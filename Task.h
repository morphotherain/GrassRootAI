#include "D3DManager.h"
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <any>

class GameObject;

struct Task {
	Task();
	~Task() {};

	UINT taskID;
	UINT innerID;
	bool isInnerTask;
	std::shared_ptr<GameObject> publisher;
	std::shared_ptr<GameObject> target;
	int publisherId = -1;
	int targetId = -1;
	int taskTypeId = -1;
	int taskStatus;
	int createdTime;
	int result;

	std::shared_ptr<std::unordered_map<std::string, std::any>> paramsPtr;

	std::string to_string() const;
	std::string to_string_abstract() const;

	template<typename T>
	T getParamOrDefault(const std::string& paramName, const T& defaultValue) const {
		if (paramsPtr->find(paramName) != paramsPtr->end()) {
			try {
				return std::any_cast<T>((*paramsPtr)[paramName]);
			}
			catch (const std::bad_any_cast& e) {
				DEBUG_("类型转换错误: {}", e.what());
			}
		}
		return defaultValue;
	}
};