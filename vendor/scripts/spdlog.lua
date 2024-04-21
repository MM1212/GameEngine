local spdlog = Vendors.spdlog;

local function sFiles(...)
  return path.join(spdlog:getPath(), ...);
end

project "spdlog"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"

  targetdir(PROJECT_VENDOR_DIR)
  objdir(PROJECT_OBJ_DIR)

  includedirs {
    sFiles 'include'
  }

  files {
    sFiles "src/**.cpp",
    sFiles "include/**.h"
  }

  defines "SPDLOG_COMPILED_LIB"

  filter "system:linux"
    systemversion "latest"
    staticruntime "On"
    pic "On"

  filter "system:windows"
    systemversion "latest"
    staticruntime "On"

  filter "configurations:debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:release"
		runtime "Release"
		optimize "speed"
