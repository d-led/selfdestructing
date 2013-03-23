local OS=os.get()

local definitions = {
	dir = {
		linux = "ls",
		windows = "dir"
	},
	BOOST =  {
		linux = "/home/dled/src/boost_1_51_0", --customize
		windows = os.getenv("BOOST")
	},
	BOOST_LIB = {
		linux = path.join("/home/dled/src/boost_1_51_0/","stage/lib"), --customize
		windows = path.join(os.getenv("BOOST"),"stage/lib")
	},
	links = {
		linux = "",
		windows=""
	}
}

local cfg={}

for i,v in pairs(definitions) do
 cfg[i]=definitions[i][OS]
end

-- Apply to current "filter" (solution/project)
function DefaultConfig()
	location "Build"
	configuration "Debug"
		defines { "DEBUG", "_DEBUG" }
		objdir "Build/obj"
		targetdir "Build/Debug"
		flags { "Symbols" }
	configuration "Release"
		defines { "RELEASE" }
		objdir "Build/obj"
		targetdir "Build/Release"
		flags { "Optimize" }
	configuration "*" -- to reset configuration filter
end

function CompilerSpecificConfiguration()
	configuration {"xcode*" }
		postbuildcommands {"$TARGET_BUILD_DIR/$TARGET_NAME"}

	configuration {"gmake"}
		postbuildcommands  { "$(TARGET)" }
		buildoptions { "-std=gnu++0x" }

	configuration {"codeblocks" }
		postbuildcommands { "$(TARGET_OUTPUT_FILE)"}

	configuration { "vs*"}
		postbuildcommands { "\"$(TargetPath)\"" }
end

-- A solution contains projects, and defines the available configurations
local sln=solution "selfdestructing"
    location "Build"
	sln.absbasedir=path.getabsolute(sln.basedir)
	configurations { "Debug", "Release" }
	platforms { "native" }
	includedirs {
		path.join(sln.basedir,"selfdestructing"),
		cfg.BOOST
	}
	vpaths {
		["Headers"] = "**.h",
		["Sources"] = {"**.cc", "**.cpp"},
	}

----------------------------------------------------------------------------------------------------------------

local tests=project "selfdestructing"
	local basedir="selfdestructing"
	kind "ConsoleApp"
	DefaultConfig()
	language "C++"
	files {
		path.join(basedir,"**.cpp"),
		path.join(basedir,"**.h"),
		path.join(basedir,"**.hpp")
	}
	uses {
	}
	links {
	--	cfg.links
	}
	CompilerSpecificConfiguration()

