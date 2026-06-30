#pragma once

#include "logger_manager.h"

// 边界日志：只在生命周期 / Task 分发 / 场景切换 / 存档 等边界打 INFO/WARN。
// 格式约定见 Document/杂项/日志设计.md

#define LOG_SIM(...)   INFO_("[Sim] " __VA_ARGS__)
#define LOG_TASK(...)  INFO_("[Task] " __VA_ARGS__)
#define LOG_SAVE(...)  INFO_("[Save] " __VA_ARGS__)
#define LOG_SCENE(...) INFO_("[Scene] " __VA_ARGS__)

#define WARN_TASK(...) WARN_("[Task] " __VA_ARGS__)
#define WARN_SIM(...)  WARN_("[Sim] " __VA_ARGS__)
