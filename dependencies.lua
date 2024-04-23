function vendorFiles(lib)
  local vendorPath = path.join(ROOT_DIR, "vendor", lib)
  return function(...)
    return path.join(vendorPath, ...);
  end
end

function vendor(lib)
  include (path.join("vendor","scripts", lib .. ".lua"))
end

local MPDepTrack = require 'vendor.scripts.mpDependencyTrack'


VULKAN_SDK = os.getenv("VULKAN_SDK")


--- @class MPVendors
Vendors = {}

Vendors.entt = MPDepTrack.new('entt')
  :addInclude("include")

Vendors.glfw = MPDepTrack.new('glfw')
  :addInclude("include")
  :addLink("GLFW");

Vendors.glm = MPDepTrack.new('glm')
  :addInclude("");

Vendors.Vulkan = MPDepTrack.new('vulkan', VULKAN_SDK)
  :addInclude('win32', "Include")
  :addLink('win32', "Lib/vulkan-1.lib");

Vendors.spdlog = MPDepTrack.new('spdlog')
  :addInclude("include")
  :addLink("spdlog")

Vendors.yaml_cpp = MPDepTrack.new('yaml-cpp')
  :addInclude("include")
  :addLink("yaml-cpp")

Vendors.ImGui = MPDepTrack.new('imgui')
  :addInclude("")
  :addLink("ImGui")

Vendors.ImGuizmo = MPDepTrack.new('ImGuizmo')
  :addInclude("")

Vendors.Engine = MPDepTrack.new('engine', '%{wks.location}/Engine')
  :addInclude("includes")