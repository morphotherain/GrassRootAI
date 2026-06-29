// HandlerFactory.h
#pragma once
#include "SolarSystemHandler.h"
#include "RefiningSystemHandler.h"
#include "SolarSystemMgr.h"
#include "RefiningSystem.h"
#include <memory>

class HandlerFactory {
public:
    static void initializeHandlers() {
        SolarSystemMgr::getInstance().registerHandler(std::make_unique<CreateObjectHandler>());
        SolarSystemMgr::getInstance().registerHandler(std::make_unique<TransferObjectHandler>());
        SolarSystemMgr::getInstance().registerHandler(std::make_unique<DestroyObjectHandler>());
        RefiningSystem::getInstance().registerHandler(std::make_unique<RefiningObjectHandler>());
    }
};
