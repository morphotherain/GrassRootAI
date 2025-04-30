#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "AttributeCalculator.h"

class AttributeCalculatorManager {
public:
    static AttributeCalculatorManager& getInstance();
    std::shared_ptr<AttributeCalculator> getCalculator(int type);

private:
    AttributeCalculatorManager() = default;
    std::unordered_map<int, std::shared_ptr<AttributeCalculator>> calculatorMap;
    std::string rulesDirectory = "resource/rules/";
    std::string getJsonFilePath(int type);
};