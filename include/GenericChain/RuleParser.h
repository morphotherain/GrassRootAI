#pragma once
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <include/thirdparty/nlohmann/json.hpp>
#include "GenericChainNode.h"
#include "CustomNode.h"

class RuleParser {
public:
    static std::shared_ptr<GenericChainNode> parseRules(const std::vector<nlohmann::json>& rules);
};