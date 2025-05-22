#include "Task.h"

Task::Task()
{
    paramsPtr = std::make_shared<std::unordered_map<std::string, std::any>>();
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

    if (paramsPtr) {
        oss << "Params:\n";
        for (const auto& pair : *paramsPtr) {
            oss << "  " << pair.first << ": ";
            const auto& param = pair.second;
            if (param.type() == typeid(int)) {
                oss << std::any_cast<int>(param) << "\n";
            }
            else if (param.type() == typeid(float)) {
                oss << std::any_cast<float>(param) << "\n";
            }
            else if (param.type() == typeid(double)) {
                oss << std::any_cast<double>(param) << "\n";
            }
            else if (param.type() == typeid(std::string)) {
                oss << std::any_cast<std::string>(param) << "\n";
            }
            else if (param.type() == typeid(std::wstring)) {
                oss << std::string(std::any_cast<std::wstring>(param).begin(),
                    std::any_cast<std::wstring>(param).end()) << "\n";
            }
            else {
                oss << "Unknown type\n";
            }
        }
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