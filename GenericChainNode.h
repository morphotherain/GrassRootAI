#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <optional>
#include "dogmaAttributesManager.h"

class GenericChainNode {
public:
    GenericChainNode(const std::string& operation, int attrId, std::optional<std::string> multiplierZone);
    virtual ~GenericChainNode() = default;
    virtual double process(double currentValue, const std::unordered_map<int, Attribute>& objectAttributes, std::unordered_map<std::string, double>& multiplierZones);
    void setNext(std::shared_ptr<GenericChainNode> next);
    double handle(double currentValue, const std::unordered_map<int, Attribute>& objectAttributes, std::unordered_map<std::string, double>& multiplierZones);

protected:
    std::string operation;
    int attrId;
    std::optional<std::string> multiplierZone;
    std::shared_ptr<GenericChainNode> next;
};