--- @alias Platform "win32" | "linux" | "macos" | "shared"
--- @type Platform[]
local platforms = {"win32", "linux", "macos"}

--- @class MultiPlatformDependencyTrackShared
--- @field public include string
--- @field public libdir string
--- @field public link string

--- @class MultiPlatformDependencyTrack
--- @field public name string
--- @field public vendorPath string
--- @field private includes table<Platform, string>
--- @field private libdirs table<Platform, string>
--- @field private links table<Platform, string>
--- @field private vendor boolean
--- @field public shared MultiPlatformDependencyTrackShared
local MultiPlatformDependencyTrack = {}

MultiPlatformDependencyTrack.__index = function(self, key)
  if key == "shared" then
    return {
      include = self:getInclude(),
      libdir = self:getLibDir(),
      link = self:getLink()
    };
  end
  return MultiPlatformDependencyTrack[key]
end

--- @param vendorName string
--- @param vendorPath string?
--- @return MultiPlatformDependencyTrack
function MultiPlatformDependencyTrack.new(vendorName, vendorPath)
  local self = setmetatable({
    name = vendorName
  }, MultiPlatformDependencyTrack)
  self.includes = {}
  self.libdirs = {}
  self.links = {}
  if not vendorPath then
    vendorPath = vendorFiles(vendorName)();
  end
  self.vendorPath = vendorPath;
  return self
end

function MultiPlatformDependencyTrack:__tostring()
  return self.name
end

--- @param platform Platform
--- @param includePath string
--- @overload fun(self: MultiPlatformDependencyTrack, includePath: string):MultiPlatformDependencyTrack
--- @return MultiPlatformDependencyTrack
function MultiPlatformDependencyTrack:addInclude(platform, includePath)
  if not includePath and not platform then
    error("addInclude: No platform or includePath specified")
  end
  if not includePath then
    includePath = platform
    platform = "shared"
  end
  self.includes[platform] = includePath
  return self
end

--- @param platform Platform
--- @param libdir string
--- @overload fun(self: MultiPlatformDependencyTrack, libdir: string):MultiPlatformDependencyTrack
--- @return MultiPlatformDependencyTrack
function MultiPlatformDependencyTrack:addLibDir(platform, libdir)
  if not libdir and not platform then
    error("addLibDir: No platform or libdir specified")
  end
  if not libdir then
    libdir = platform
    platform = "shared"
  end
  self.libdirs[platform] = libdir
  return self
end

--- @param platform Platform
--- @param link string
--- @overload fun(self: MultiPlatformDependencyTrack, link: string):MultiPlatformDependencyTrack
--- @return MultiPlatformDependencyTrack
function MultiPlatformDependencyTrack:addLink(platform, link)
  if not link and not platform then
    error("addLink: No platform or link specified")
  end
  if not link then
    link = platform
    platform = "shared"
  end
  self.links[platform] = link
  return self
end

--- @param platform Platform?
--- @return string
function MultiPlatformDependencyTrack:getInclude(platform)
  if not platform then
    platform = "shared"
  end
  return path.join(self:getPath(), self.includes[platform] or "")
end

--- @param platform Platform?
--- @return string
function MultiPlatformDependencyTrack:getLibDir(platform)
  if not platform then
    platform = "shared"
  end
  return path.join(self:getPath(), self.libdirs[platform] or "")
end

--- @param platform Platform?
--- @param addPath boolean?
--- @overload fun(self: MultiPlatformDependencyTrack, addPath: boolean?):string
--- @return string
function MultiPlatformDependencyTrack:getLink(platform, addPath)
  if addPath == nil then
    addPath = false
  end
  if platform == nil then
    if type(platform) == "boolean" then
      addPath = platform
    end
    platform = "shared"
  end
  local link = self.links[platform] or ""
  if addPath then
    return path.join(self:getPath(), link)
  end
  return link
end

function MultiPlatformDependencyTrack:getPath()
  return self.vendorPath;
end

return MultiPlatformDependencyTrack;