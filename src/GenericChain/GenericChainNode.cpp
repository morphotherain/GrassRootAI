#include "GenericChainNode.h"

GenericChainNode::GenericChainNode(const std::string& operation, int attrId, std::optional<std::string> multiplierZone)
    : operation(operation), attrId(attrId), multiplierZone(multiplierZone) {}

double GenericChainNode::process(double currentValue, const std::unordered_map<int, Attribute>& objectAttributes, std::unordered_map<std::string, double>& multiplierZones) {
    auto it = objectAttributes.find(attrId);
    if (it == objectAttributes.end()) {
        return currentValue;
    }
    double attrValue = it->second.value;

    if (operation == "add") {
        if (!multiplierZone) {
            currentValue += attrValue;
        }
        else {
            multiplierZones[*multiplierZone] += attrValue;
        }
    }
    else if (operation == "multiply") {
        if (!multiplierZone) {
            currentValue *= attrValue;
        }
        else {
            multiplierZones[*multiplierZone] *= attrValue;
        }
    }
    return currentValue;
}

void GenericChainNode::setNext(std::shared_ptr<GenericChainNode> next) {
    this->next = next;
}

double GenericChainNode::handle(double currentValue, const std::unordered_map<int, Attribute>& objectAttributes, std::unordered_map<std::string, double>& multiplierZones) {
    double newValue = process(currentValue, objectAttributes, multiplierZones);
    if (next) {
        return next->handle(newValue, objectAttributes, multiplierZones);
    }
    return newValue;
}