-- Minimal premake5.lua for selfdestructing project

workspace "selfdestructing"
    architecture "x86_64"
    
    configurations { "Debug", "Release" }
    platforms { "x64", "x32" }
    
    -- Output directories to match existing structure
    location "Build/%{os.target()}/%{_ACTION}"
    
    filter "platforms:x32"
        architecture "x86"
    filter "platforms:x64"
        architecture "x86_64"
    filter {}

project "selfdestructing-test"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    
    -- Source files
    files { "test/**.cpp" }
    
    -- Include directories
    includedirs { 
        "selfdestructing",
        "Catch/single_include"
    }
    
    -- Output directories to match existing structure
    targetdir "bin/%{os.target()}/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}"
    objdir "Build/%{os.target()}/%{_ACTION}/obj/%{cfg.platform}/%{cfg.buildcfg}"
    
    filter "configurations:Debug"
        defines { "_DEBUG" }
        symbols "On"
        optimize "Off"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        symbols "Off"
        optimize "Full"
    
    filter {}
    
    -- Post-build command to run the test (matches existing makefile behavior)
    postbuildcommands {
        "%{cfg.buildtarget.abspath}"
    }
