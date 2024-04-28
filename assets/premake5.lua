ASSETS_TARGET_DIR = path.join(ROOT_DIR, "bin", OUTPUT_DIR, "assets" , "%{prj.name}")
ASSETS_OBJ_DIR = path.join(ROOT_DIR, "objs", OUTPUT_DIR, "assets" , "%{prj.name}")

project "assets"
  kind "Utility"
  targetdir (PROJECT_TARGET_DIR)
  objdir (PROJECT_OBJ_DIR)
  files {
    "models/**.obj",
  }
  filter { "files:**"}
    buildmessage "Copy %{file.relpath} to %{cfg.targetdir}/%{file.relpath}"
    buildcommands {
      "{COPYFILE} %{file.relpath} %[%{cfg.targetdir}/%{file.relpath}]"
    }
    buildoutputs {
      "%{cfg.targetdir}/%{file.relpath}"
    }

  include "shaders"