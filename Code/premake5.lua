workspace "DarkSunMud"
	architecture "x64"

	configurations
	{
		"Debug",
		"Dev",
		"Prod",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "DarkSunServer"
	location "DarkSunServer"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "dsspch.h"
	pchsource "DarkSunServer/src/dsspch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/asio-1.28.0/include",
		"%{prj.name}/vendor/spdlog-1.12.0/include",
	}

	links
	{
		"libssl.lib",
		"libcrypto.lib"
	}

	debugdir "devdir"

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"DSS_PLATFORM_WINDOWS"
		}

		includedirs
		{
			"%{prj.name}/platform-vendor/openssl/windows/include",
		}

		libdirs
		{
			"%{prj.name}/platform-vendor/openssl/windows/lib"
		}
		
		postbuildcommands
		{
			("{COPY} platform-vendor/openssl/windows/bin ../bin/" .. outputdir .. "/%{prj.name}"),
		}

	filter "configurations:Debug"
		defines "DSS_DEBUG"
		symbols "On"

	filter "configurations:Dev"
		defines "DSS_DEV"
		optimize "On"

	filter "configurations:Prod"
		defines "DSS_PROD"
		optimize "On"