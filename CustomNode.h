#pragma once
#include "GenericChainNode.h"

class CustomNode : public GenericChainNode {
public:
    CustomNode(int param);
    double process(double currentValue, const std::unordered_map<int, Attribute>& objectAttributes, std::unordered_map<std::string, double>& multiplierZones) override;

private:
    int param;
};