#include "engine/utils/logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <sstream>

#define COLOR_RED "\033[31m"
#define COLOR_CYAN "\033[36m"
#define COLOR_RESET "\033[0m"

namespace Engine {
  std::shared_ptr<spdlog::logger> Logger::MainLogger;
  std::shared_ptr<spdlog::logger> Logger::AppLogger;

  void Logger::Init() {
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::stringstream pattern;
    pattern
      << "[" << COLOR_RED << "%T" << COLOR_RESET << "] "
      << "[" << COLOR_CYAN << "%-6n" << COLOR_RESET << "] "
      << "[%^%l%$] %v";
    consoleSink->set_pattern(pattern.str());
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("engine.log", true);

    MainLogger = std::make_shared<spdlog::logger>("Engine", spdlog::sinks_init_list{ consoleSink, fileSink });
    MainLogger->set_level(spdlog::level::trace);
    MainLogger->flush_on(spdlog::level::trace);
    spdlog::register_logger(MainLogger);

    AppLogger = std::make_shared<spdlog::logger>("App", spdlog::sinks_init_list{ consoleSink, fileSink });
    AppLogger->set_level(spdlog::level::trace);
    AppLogger->flush_on(spdlog::level::trace);
    spdlog::register_logger(AppLogger);
    LOG_TRACE("Logger initialized");
  }
}