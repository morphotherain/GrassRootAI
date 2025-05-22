#include "AttributeCalculatorManager.h"
#include <sstream>

AttributeCalculatorManager& AttributeCalculatorManager::getInstance() {
    static AttributeCalculatorManager instance;
    return instance;
}

std::string AttributeCalculatorManager::getJsonFilePath(int type) {
    std::ostringstream oss;
    oss << rulesDirectory << "rules" << type << ".json";
    return oss.str();
}

std::shared_ptr<AttributeCalculator> AttributeCalculatorManager::getCalculator(int type) {
    auto it = calculatorMap.find(type);
    if (it != calculatorMap.end()) {
        return it->second;
    }

    std::string jsonFilePath = getJsonFilePath(type);
    auto calculator = std::make_shared<AttributeCalculator>(jsonFilePath);
    calculatorMap[type] = calculator;
    return calculator;
}