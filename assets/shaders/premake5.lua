project "shaders"
  kind "Utility"
  targetdir (ASSETS_TARGET_DIR)
  objdir (ASSETS_OBJ_DIR)
  files {
    "**.vert",
    "**.frag"
  }
  filter { "files:**.vert or **.frag"}
    buildmessage "Compiling %{file.relpath} to %{cfg.targetdir}/%{file.name:gsub('.glsl', '')}.spv"
    buildcommands {
      "{MKDIR} %[%{cfg.targetdir}]",
      "glslc %[%{file.relpath}] -o %[%{cfg.targetdir}/%{file.name:gsub('.glsl', '')}.spv]"
    }
    buildoutputs {
      "%{cfg.targetdir}/%{file.name:gsub('.glsl', '')}.spv"
    }