#include "D3DManager.h"
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <any>

class GameObject;

enum SystemType : int {
	NONE = 0,           // 明确表示0为默认值
	SOLAR_SYSTEM,       // 1
	MARKET,             // 2
	UIWINDOW,             // 2
    // 添加一个最大值用于确定数组大小
	MAX_SYSTEM_TYPE
};


struct Task {
	Task();
	~Task() {};

	UINT taskID;
	UINT innerID;
	bool isInnerTask = true;
	std::weak_ptr<GameObject> publisher;
	std::weak_ptr<GameObject> target;
	int publisherId = -1;
	int targetId = -1;
	int taskTypeId = -1;
	int taskStatus;
	int createdTime;
	int result;
	SystemType targetSystem = SystemType::NONE;

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