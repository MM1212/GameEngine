project 'stb_image'
  kind 'StaticLib'
  language 'C'
  staticruntime 'on'

  targetdir (PROJECT_VENDOR_DIR)
  objdir (PROJECT_OBJ_DIR)

  files {
    "includes/**.h",
    "src/**.c"
  }

  includedirs {
    "includes"
  }

  filter "system:windows"
    systemversion "latest"

  filter "system:linux"
    pic "On"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    runtime "Release"
    optimize "on"