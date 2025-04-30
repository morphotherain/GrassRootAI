#pragma once
#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <vector>
#include <include/spdlog/spdlog.h>
#include <include/spdlog/sinks/basic_file_sink.h>
#include <include/spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <iomanip>
#include <sstream>

// 辅助函数：从完整路径中提取文件名
std::string formatFileInfo(const char* filePath, int line, const char* function);

#define DEBUG_(...) \
    spdlog::debug("{} {}", formatFileInfo(__FILE__, __LINE__, __FUNCTION__), fmt::format(__VA_ARGS__))
//这是我的dubug宏，能不能为上面的那些逻辑加上方便调试的中文debug信息呢

// 自定义 INFO 宏
#define INFO_(...) \
    spdlog::info("{} {}", formatFileInfo(__FILE__, __LINE__, __FUNCTION__), fmt::format(__VA_ARGS__))

// 自定义 WARN 宏
#define WARN_(...) \
    spdlog::warn("{} {}", formatFileInfo(__FILE__, __LINE__, __FUNCTION__), fmt::format(__VA_ARGS__))

// 自定义 ERROR 宏
#define ERROR_(...) \
    spdlog::error("{} {}", formatFileInfo(__FILE__, __LINE__, __FUNCTION__), fmt::format(__VA_ARGS__))

// 初始化日志系统
void initLogging();

// 关闭日志系统
void shutdownLogging();

#endif // LOGGING_H