project "Sandbox"
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
    "%{Vendors.entt.shared.include}",
	}

	links {
		"Engine"
	}

  filter "system:windows"
    defines { '_WIN32' }

  filter "system:linux"
    defines { '_LINUX' }

	filter "system:windows"
		systemversion "latest"
  
  filter "system:linux"
    pic "On"
    systemversion "latest"

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"