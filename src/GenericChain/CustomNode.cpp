#include "CustomNode.h"

CustomNode::CustomNode(int param) : GenericChainNode("", 0, std::nullopt), param(param) {}

double CustomNode::process(double currentValue, const std::unordered_map<int, Attribute>& objectAttributes, std::unordered_map<std::string, double>& multiplierZones) {
    // 自定义处理逻辑
    return currentValue + param;
}