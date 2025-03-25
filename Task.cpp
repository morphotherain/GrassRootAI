#include "Task.h"

Task::Task()
{
	intParamsPtr = std::make_shared<std::vector<int>>();
	stringParamsPtr = std::make_shared<std::vector<std::string>>();
	wstringParamsPtr = std::make_shared<std::vector<std::wstring>>();
}

std::string Task::to_string() const
{
    std::ostringstream oss;
    oss << "Task ID: " << taskID << "\n";
    oss << "Inner ID: " << innerID << "\n";
    oss << "Is Inner Task: " << (isInnerTask ? "Yes" : "No") << "\n";
    oss << "Task Type ID: " << taskTypeId << "\n";
    oss << "Task Status: " << taskStatus << "\n";
    oss << "Created Time: " << createdTime << "\n";
    oss << "Result: " << result << "\n";

    if (intParamsPtr) {
        oss << "Int Params: ";
        for (int param : *intParamsPtr) {
            oss << param << " ";
        }
        oss << "\n";
    }

    if (floatParamsPtr) {
        oss << "Float Params: ";
        for (float param : *floatParamsPtr) {
            oss << param << " ";
        }
        oss << "\n";
    }

    if (doubleParamsPtr) {
        oss << "Double Params: ";
        for (double param : *doubleParamsPtr) {
            oss << param << " ";
        }
        oss << "\n";
    }

    if (stringParamsPtr) {
        oss << "String Params: ";
        for (const std::string& param : *stringParamsPtr) {
            oss << param << " ";
        }
        oss << "\n";
    }

    if (wstringParamsPtr) {
        oss << "WString Params: ";
        for (const std::wstring& param : *wstringParamsPtr) {
            // 这里简单处理，实际中可能需要更复杂的转换
            oss << std::string(param.begin(), param.end()) << " ";
        }
        oss << "\n";
    }

    return oss.str();
}

std::string Task::to_string_abstract() const
{
    std::ostringstream oss;
    oss << "TaskID:" << taskID << ", InnerID:" << innerID
        << ", IsInner:" << (isInnerTask ? "Yes" : "No")
        << ", TypeID:" << taskTypeId;
    return oss.str();
}
