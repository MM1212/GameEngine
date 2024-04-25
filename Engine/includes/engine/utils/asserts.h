#pragma once

#ifdef _DEBUG
	#if defined(_WIN32)
		#define _DEBUGBREAK() __debugbreak()
	#elif defined(_LINUX)
		#include <signal.h>
		#define _DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define ENABLE_ASSERTS
#else
	#define _DEBUGBREAK()
#endif

#ifdef ENABLE_ASSERTS
  #include "logger.h"
  #define ASSERT(x, ...) { if(!(x)) { LOG_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); _DEBUGBREAK(); } }
  #define ASSERT_MSG(x, msg, ...) { if(!(x)) { LOG_CRITICAL(msg, __VA_ARGS__); _DEBUGBREAK(); } }
  #define APP_ASSERT(x, ...) { if(!(x)) { LOG_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); _DEBUGBREAK(); } }
#else
  #define ASSERT(x, ...)
  #define ASSERT_MSG(x, msg, ...)
  #define APP_ASSERT(x, ...)
#endif