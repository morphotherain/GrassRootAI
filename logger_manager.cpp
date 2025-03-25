#include "logger_manager.h"

// 初始化日志系统
void initLogging() {
    try {
        // 创建控制台输出的日志接收器
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::debug);
        consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

        // 创建文件输出的日志接收器
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/game.log", true);
        fileSink->set_level(spdlog::level::trace);
        fileSink->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

        // 创建一个多线程的日志记录器
        std::vector<spdlog::sink_ptr> sinks{ consoleSink, fileSink };
        auto logger = std::make_shared<spdlog::logger>("game_logger", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);

        // 注册日志记录器
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "日志初始化失败: " << ex.what() << std::endl;
    }
}

// 关闭日志系统
void shutdownLogging() {
    spdlog::shutdown();
}

// 辅助函数：从完整路径中提取文件名
std::string formatFileInfo(const char* filePath, int line, const char* function) {
    const char* fileName = filePath;
    const char* lastSlash = nullptr;
    while (*filePath) {
        if (*filePath == '/' || *filePath == '\\') {
            lastSlash = filePath + 1;
        }
        ++filePath;
    }
    if (lastSlash) {
        fileName = lastSlash;
    }

    std::ostringstream oss;
    oss << "[" << std::setw(15) << std::left << fileName << ":"
        << std::setw(4) << std::right << line << ":"
        << std::setw(10) << std::left << function << "]";
    return oss.str();
}