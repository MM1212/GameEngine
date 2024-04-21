local imgui = Vendors.ImGui;

local function sFiles(...)
  return path.join(imgui:getPath(), ...);
end

project "ImGui"
	kind "StaticLib"
	language "C++"
  cppdialect "C++20"
  staticruntime "On"

	targetdir (PROJECT_VENDOR_DIR)
	objdir (PROJECT_OBJ_DIR)

	files {
		sFiles "imconfig.h",
		sFiles "imgui.h",
		sFiles "imgui.cpp",
		sFiles "imgui_draw.cpp",
		sFiles "imgui_internal.h",
		sFiles "imgui_widgets.cpp",
		sFiles "imstb_rectpack.h",
		sFiles "imstb_textedit.h",
		sFiles "imstb_truetype.h",
    sFiles "imgui_tables.cpp"
  }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
    files {
      sFiles "imgui_demo.cpp"
    }

	filter "configurations:Release"
		runtime "Release"
		optimize "on"