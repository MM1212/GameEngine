project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir(PROJECT_TARGET_DIR)
	objdir(PROJECT_OBJ_DIR)

	files {
		"includes/**.h",
		"src/**.cpp"
	}

	includedirs {
    "includes",
    "%{Vendors.Engine.shared.include}",
    "%{Vendors.spdlog.shared.include}",
    "%{Vendors.glm.shared.include}",
    "%{Vendors.ImGuizmo.shared.include}",
    "%{Vendors.entt.shared.include}",
	}

	links {
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"
    defines { '_WIN32' }

  filter "system:linux"
    pic "On"
    systemversion "latest"
    defines { '_LINUX' }
    -- since gmake2 doesn't link agaisnt Engine dependencies, we have to do that ourselves
    links {
      "GLFW",
      "ImGui",
      "yaml-cpp",
      "spdlog",
      "stb_image"
    }

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"