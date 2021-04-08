project "Warp-Application"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "../Warp-Core/src",
        "../Warp-Core/vendor/spdlog/include"
    }

    links {
        "Warp-Core"
    }

    filter "system:windows"
		systemversion "latest"

		defines {
			"WARP_CORE_PLATFORM_WINDOWS"
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