#include "RuleParser.h"

std::unordered_map<std::string, std::function<std::shared_ptr<GenericChainNode>(const nlohmann::json&)>> customOperationMap = {
    {
        "custom_operation",
        [](const nlohmann::json& rule) {
            int param = rule["parameters"]["param"].get<int>();
            return std::make_shared<CustomNode>(param);
        }
    }
};

std::shared_ptr<GenericChainNode> RuleParser::parseRules(const std::vector<nlohmann::json>& rules) {
    if (rules.empty()) {
        return nullptr;
    }

    std::shared_ptr<GenericChainNode> head;
    std::shared_ptr<GenericChainNode> current;

    for (const auto& rule : rules) {
        std::string operation = rule["operation"].get<std::string>();
        if (customOperationMap.find(operation) != customOperationMap.end()) {
            auto node = customOperationMap[operation](rule);
            if (!head) {
                head = node;
                current = node;
            }
            else {
                current->setNext(node);
                current = node;
            }
        }
        else {
            int attrId = rule["attrId"].get<int>();
            std::optional<std::string> multiplierZone;
            if (!rule["multiplierZone"].is_null()) {
                multiplierZone = rule["multiplierZone"].get<std::string>();
            }
            else {
                multiplierZone = std::nullopt;
            }
            auto node = std::make_shared<GenericChainNode>(operation, attrId, multiplierZone);
            if (!head) {
                head = node;
                current = node;
            }
            else {
                current->setNext(node);
                current = node;
            }
        }
    }

    return head;
}