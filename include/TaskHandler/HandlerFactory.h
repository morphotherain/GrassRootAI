// HandlerFactory.h
#pragma once
#include "SolarSystemHandler.h"

class HandlerFactory {
public:
    // 初始化所有Handler
    static void initializeHandlers() {
        // 创建并注册Handler（触发构造函数中的注册）
        new CreateObjectHandler();
        new TransferObjectHandler();
        //new DestroyObjectHandler();
        // ...其他Handler
    }
};