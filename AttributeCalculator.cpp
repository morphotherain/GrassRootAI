#include "AttributeCalculator.h"
#include <fstream>

AttributeCalculator::AttributeCalculator(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    nlohmann::json jsonRules;
    file >> jsonRules;

    std::vector<nlohmann::json> rules = jsonRules["rules"];
    chainHead = RuleParser::parseRules(rules);
}

double AttributeCalculator::calculate(const std::unordered_map<int, Attribute>& objectAttributes) {
    if (!chainHead) {
        return 0.0;
    }
    std::unordered_map<std::string, double> multiplierZones;
    double baseValue = chainHead->handle(0.0, objectAttributes, multiplierZones);

    for (const auto& zone : multiplierZones) {
        baseValue *= zone.second;
    }
    return baseValue;
}