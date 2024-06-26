
OUTPUT_DIR = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
ROOT_DIR = path.getabsolute(".")
PROJECT_TARGET_DIR = path.join(ROOT_DIR, "bin", OUTPUT_DIR, "%{prj.name}")
PROJECT_VENDOR_DIR = path.join(ROOT_DIR, "bin", OUTPUT_DIR, "vendor", "%{prj.name}")
PROJECT_OBJ_DIR = path.join(ROOT_DIR, "objs", OUTPUT_DIR, "%{prj.name}")

include "dependencies.lua"

workspace "GameEngine"
  architecture "x64"
  startproject "Editor"
  configurations { "Debug", "Release" }
  flags { "MultiProcessorCompile" }
  defines {
    "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    "GLM_FORCE_RADIANS",
    'SPDLOG_NO_THREAD_ID',
    "SPDLOG_COMPILED_LIB",
    "YAML_CPP_STATIC_DEFINE"
  }
  filter "configurations:Debug"
    defines {
      "ENABLE_ENGINE_LOGGING",
      "ENABLE_APP_LOGGING"
    }
  filter "configurations:Release"
    defines {
      "ENABLE_APP_LOGGING"
    }
  filter "system:linux"
    buildoptions { "-gdwarf-2" }
    defines {
      'SPDLOG_CLOCK_COARSE'
    }
  filter {}

group "Vendor"
  vendor "glfw"
  vendor "yaml-cpp"
  vendor "imgui"
  vendor "spdlog"
  include "vendor/stb_image"
group ""

group "Core"
  include "Engine"
  include "assets"
group ""

group "Tools"
  include "Editor"
group ""

group "Runtime"
  include "Sandbox"
group ""