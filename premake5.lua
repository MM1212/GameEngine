
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

group "Vendor"
  vendor "glfw"
  vendor "yaml-cpp"
  vendor "imgui"
  vendor "spdlog"
group ""

group "Core"
  include "Engine"
group ""

group "Tools"
  include "Editor"
group ""

group "Runtime"
  include "Sandbox"
group ""