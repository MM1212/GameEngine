print(Vendors.ImGui.shared.include)

project "Engine"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "off"

  targetdir (PROJECT_TARGET_DIR)
  objdir (PROJECT_OBJ_DIR)

  files {
    "includes/**.h",
    "src/**.cpp",
    "shaders/**.vert",
    "shaders/**.frag",
    "%{Vendors.ImGuizmo:getPath()}/ImGuizmo.cpp"
  }

  defines {
    "_CRT_SECURE_NO_WARNINGS",
    "GLFW_INCLUDE_NONE",
    "GLM_FORCE_RADIANS",
    "GLM_FORCE_DEPTH_ZERO_TO_ONE"
  }

  includedirs {
    "includes",
    "includes/engine",
    "%{Vendors.spdlog.shared.include}",
    "%{Vendors.glm.shared.include}",
    "%{Vendors.glfw.shared.include}",
    "%{Vendors.ImGui.shared.include}",
    "%{Vendors.ImGuizmo.shared.include}",
    "%{Vendors.entt.shared.include}",
    "%{Vendors.yaml_cpp.shared.include}",
    "%{Vendors.Vulkan.shared.include}"
  }

  links {
    "GLFW",
    "ImGui",
    "yaml-cpp",
    "spdlog"
  }

  filter "system:windows"
    defines { '_WIN32' }
  
  filter "system:linux"
    defines { '_LINUX' }

  filter "system:windows"
    systemversion "latest"
    includedirs {
      "%{Vendors.Vulkan:getInclude('win32')}"
    }
    links {
      "%{Vendors.Vulkan:getLink('win32', true)}"
    }

  filter "system:linux"
    pic "On"
    systemversion "latest"
    links {
      "%{Vendors.Vulkan:getLink('linux')}"
    }

  filter "configurations:Debug"
    defines "_DEBUG"
    runtime "Debug"
    symbols "on"


  filter "configurations:Release"
    defines "_RELEASE"
    runtime "Release"
    optimize "on"
