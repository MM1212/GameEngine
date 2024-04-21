-- https://github.com/TheCherno/glfw/blob/master/premake5.lua

local glfw = Vendors.glfw;

local function sFiles(...)
  return path.join(glfw:getPath(), ...);
end

project "GLFW"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	warnings "off"

	targetdir (PROJECT_VENDOR_DIR)
	objdir (PROJECT_OBJ_DIR)

	files {
		sFiles "include/GLFW/glfw3.h",
		sFiles "include/GLFW/glfw3native.h",
		sFiles "src/glfw_config.h",
		sFiles "src/context.c",
		sFiles "src/init.c",
		sFiles "src/input.c",
		sFiles "src/monitor.c",

		sFiles "src/null_init.c",
		sFiles "src/null_joystick.c",
		sFiles "src/null_monitor.c",
		sFiles "src/null_window.c",

		sFiles "src/platform.c",
    sFiles "src/vulkan.c",
		sFiles "src/window.c",
	}

	filter "system:linux"
		pic "On"

		systemversion "latest"
		files {
			sFiles "src/x11_init.c",
			sFiles "src/x11_monitor.c",
			sFiles "src/x11_window.c",
			sFiles "src/xkb_unicode.c",
			sFiles "src/posix_module.c",
			sFiles "src/posix_time.c",
			sFiles "src/posix_thread.c",
			sFiles "src/posix_module.c",
      sFiles "src/posix_poll.c",
			sFiles "src/glx_context.c",
			sFiles "src/egl_context.c",
			sFiles "src/osmesa_context.c",
			sFiles "src/linux_joystick.c"
		}

		defines { "_GLFW_X11" }

  filter "system:windows"
		systemversion "latest"

		files
		{
			sFiles "src/win32_init.c",
			sFiles "src/win32_joystick.c",
			sFiles "src/win32_module.c",
			sFiles "src/win32_monitor.c",
			sFiles "src/win32_time.c",
			sFiles "src/win32_thread.c",
			sFiles "src/win32_window.c",
			sFiles "src/wgl_context.c",
			sFiles "src/egl_context.c",
			sFiles "src/osmesa_context.c"
		}

		defines
		{
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

		links
		{
			"Dwmapi.lib"
		}

	filter "configurations:debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:release"
		runtime "Release"
		optimize "speed"