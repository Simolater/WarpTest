workspace "Warp"
    architecture "x64"
    startproject "Warp"

    configurations { "Debug", "Release", "Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Warp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "wppch.h"
    pchsource "src/wppch.cpp"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src",
        "vendor/spdlog/include"
    }

    filter "system:windows"
		systemversion "latest"

		defines {
			"WARP_PLATFORM_WINDOWS"
		}

    filter "configurations:Debug"
        defines { "DEBUG", "WP_LOG_ENABLE" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG", "WP_LOG_ENABLE" }
        symbols "On"
        optimize "On"

    filter "configurations:Dist"
        defines { "NDEBUG" }
        optimize "On"
