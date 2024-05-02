#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

#include <memory>

// ENABLE_ENGINE_LOGGING -> Enable logging for the engine
// ENABLE_APP_LOGGING -> Enable logging for the application
#ifndef LOG_LEVEL
# define LOG_LEVEL SPDLOG_LEVEL_TRACE // Default log level, error and critical messages will always be logged
#endif

namespace Engine {

  class Logger {
  public:
    static void Init();
    inline static std::shared_ptr<spdlog::logger>& GetMainLogger() { return MainLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetAppLogger() { return AppLogger; }
  private:
    static std::shared_ptr<spdlog::logger> MainLogger;
    static std::shared_ptr<spdlog::logger> AppLogger;
  };
}

#if (defined(ENABLE_ENGINE_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_TRACE))
# define LOG_TRACE(...) ::Engine::Logger::GetMainLogger()->trace(__VA_ARGS__)
#else
# define LOG_TRACE(...)
#endif

#if (defined(ENABLE_ENGINE_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_INFO))
# define LOG_INFO(...) ::Engine::Logger::GetMainLogger()->info(__VA_ARGS__)
#else
# define LOG_INFO(...)
#endif

#if (defined(ENABLE_ENGINE_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_WARN))
# define LOG_WARN(...) ::Engine::Logger::GetMainLogger()->warn(__VA_ARGS__)
#else
# define LOG_WARN(...)
#endif

# define LOG_ERROR(...) ::Engine::Logger::GetMainLogger()->error(__VA_ARGS__)
# define LOG_CRITICAL(...) ::Engine::Logger::GetMainLogger()->critical(__VA_ARGS__)

#if (defined(ENABLE_APP_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_TRACE))
# define LOG_APP_TRACE(...) ::Engine::Logger::GetAppLogger()->trace(__VA_ARGS__)
#else
# define LOG_APP_TRACE(...)
#endif

#if (defined(ENABLE_APP_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_INFO))
# define LOG_APP_INFO(...) ::Engine::Logger::GetAppLogger()->info(__VA_ARGS__)
#else
# define LOG_APP_INFO(...)
#endif

#if (defined(ENABLE_APP_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_WARN))
# define LOG_APP_WARN(...) ::Engine::Logger::GetAppLogger()->warn(__VA_ARGS__)
#else
# define LOG_APP_WARN(...)
#endif

#define LOG_APP_ERROR(...) ::Engine::Logger::GetAppLogger()->error(__VA_ARGS__)
#define LOG_APP_CRITICAL(...) ::Engine::Logger::GetAppLogger()->critical(__VA_ARGS__)