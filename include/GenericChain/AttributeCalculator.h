#pragma once
#include <memory>
#include <unordered_map>
#include <include/thirdparty/nlohmann/json.hpp>
#include "GenericChainNode.h"
#include "RuleParser.h"

class AttributeCalculator {
public:
    AttributeCalculator(const std::string& jsonFilePath);
    double calculate(const std::unordered_map<int, Attribute>& objectAttributes);

private:
    std::shared_ptr<GenericChainNode> chainHead;
};