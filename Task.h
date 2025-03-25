#include "D3DManager.h"
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>


class GameObject;

struct Task {
    Task() ;
    ~Task() {};

    UINT taskID;
    UINT innerID;
    bool isInnerTask;
    std::shared_ptr<GameObject> publisher;
    std::shared_ptr<GameObject> target;
    UINT taskTypeId;
    UINT taskStatus;
    UINT createdTime;
    UINT result;

    std::shared_ptr<std::vector<int>> intParamsPtr;  
    std::shared_ptr<std::vector<float>> floatParamsPtr;  
    std::shared_ptr<std::vector<double>> doubleParamsPtr;  
    std::shared_ptr<std::vector<std::string>> stringParamsPtr;  
    std::shared_ptr<std::vector<std::wstring>> wstringParamsPtr;

    std::string to_string() const;
    std::string to_string_abstract() const;
};