project "Warp-Core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "wppch.h"
    pchsource "src/wppch.cpp"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src"
    }

    filter "system:windows"
		systemversion "latest"

		defines {
			"WARP_CORE_PLATFORM_WINDOWS"
		}

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        symbols "On"
        optimize "On"

    filter "configurations:Debug"
        defines { "NDEBUG" }
        optimize "On"