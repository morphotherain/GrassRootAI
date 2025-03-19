#pragma once
#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// 初始化日志系统
void initLogging();

// 关闭日志系统
void shutdownLogging();


#endif // LOGGING_H