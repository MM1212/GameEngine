-- https://github.com/TheCherno/yaml-cpp/blob/master/premake5.lua

local yaml_cpp = Vendors.yaml_cpp;

local function sFiles(...)
  return path.join(yaml_cpp:getPath(), ...);
end

project "yaml-cpp"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"

  targetdir (PROJECT_VENDOR_DIR)
  objdir (PROJECT_OBJ_DIR)

  files {
    sFiles "src/**.h",
    sFiles "src/**.cpp",
    sFiles "include/**.h"
  }

  includedirs {
    sFiles "include"
  }

  defines {
    "YAML_CPP_STATIC_DEFINE"
  }

  filter "system:windows"
    systemversion "latest"
    staticruntime "off"

  filter "system:linux"
    pic "On"
    systemversion "latest"
    staticruntime "off"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    runtime "Release"
    optimize "on"