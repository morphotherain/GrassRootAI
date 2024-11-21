#include "Task.h"

Task::Task()
{
	intParamsPtr = std::make_shared<std::vector<int>>();
	stringParamsPtr = std::make_shared<std::vector<std::string>>();
	wstringParamsPtr = std::make_shared<std::vector<std::wstring>>();
}
