#pragma once

#include "RendererAPI.h"

#if (defined(ENABLE_ENGINE_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_TRACE))
# define LOG_RENDERER_TRACE(...) ::Engine::RendererAPI::GetLogger()->trace(__VA_ARGS__)
#else
# define LOG_TRACE(...)
#endif

#if (defined(ENABLE_ENGINE_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_INFO))
# define LOG_RENDERER_INFO(...) ::Engine::RendererAPI::GetLogger()->info(__VA_ARGS__)
#else
# define LOG_INFO(...)
#endif

#if (defined(ENABLE_ENGINE_LOGGING) && (LOG_LEVEL <= SPDLOG_LEVEL_WARN))
# define LOG_RENDERER_WARN(...) ::Engine::RendererAPI::GetLogger()->warn(__VA_ARGS__)
#else
# define LOG_WARN(...)
#endif

# define LOG_RENDERER_ERROR(...) ::Engine::RendererAPI::GetLogger()->error(__VA_ARGS__)
# define LOG_RENDERER_CRITICAL(...) ::Engine::RendererAPI::GetLogger()->critical(__VA_ARGS__)