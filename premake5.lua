workspace "Everything"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release",
		"Distribution"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
project "Engine"
	location "engine"
	kind "SharedLib"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	pchheader "PCH_Engine.h"
	pchsource "%{prj.name}/src/PCH_Engine.cpp"
	characterset("MBCS")
	
	includedirs
	{	
		"%{prj.name}/src",
		"D:/coding/Vulkan/Include",
		"%{prj.name}/../vendor/GLM",
		"%{prj.name}/../"
	}

	links
	{
		"D:/coding/Vulkan/Lib/Vulkan-1"
	}

	libdirs
	{
		"bin/Debug-windows-x86_64/VulkanRenderer",
		"D:/coding/Vulkan/Lib"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		
		
		defines
		{
			PLAT_WINDOWS
		}
	
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Testapp"),
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/UnitTests")
		}
	
	filter "configurations:Debug"
		defines "VK_USE_PLATFORM_WIN32_KHR;VK_PROTOTYPES;ENGINE_DEBUG;ENGINE_DLL"
		symbols "on"
		
	filter "configurations:Release"
		defines "VK_USE_PLATFORM_WIN32_KHR;VK_PROTOTYPES;ENGINE_DIST;ENGINE_RELEASE;ENGINE_DLL"
		symbols "on"
		optimize "on"
	
	filter "configurations:Distribution"
		defines "VK_USE_PLATFORM_WIN32_KHR;VK_PROTOTYPES;ENGINE_DIST;ENGINE_DLL"
		optimize "on"

project "Testapp"
	location "Testapp"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	characterset("MBCS")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs
	{	
		"Engine/src/"
	}
	
	links
	{
		"Engine"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		
		defines
		{
			PLAT_WINDOWS
		}
	
	filter "configurations:Debug"
		defines "GAME_DEBUG"
		symbols "on"
		
	filter "configurations:Release"
		defines "GAME_RELEASE"
		symbols "on"
		optimize "on"
	
	filter "configurations:Distribution"
		defines "GAME_DIST"
		optimize "on"

project "UnitTests"
	location "UnitTests"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	characterset("MBCS")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs
	{	
		"Engine/src/"
	}
	
	links
	{
		"Engine"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		
		defines
		{
			PLAT_WINDOWS,
		}
	
	filter "configurations:Debug"
		defines "GAME_DEBUG"
		symbols "on"
		
	filter "configurations:Release"
		defines "GAME_RELEASE"
		symbols "on"
		optimize "on"
	
	filter "configurations:Distribution"
		defines "GAME_DIST"
		optimize "on"


project "VulkanRenderer"
	location "VulkanRenderer"
	kind "SharedLib"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	characterset("MBCS")
	
	includedirs
	{	
		"%{prj.name}/src",
		"%{prj.name}/../Engine/src",
		"%{prj.name}/../vendor/GLM",
		"D:/coding/Vulkan/Include"
	}

	links
	{
		"bin/Debug-windows-x86_64/Engine/Engine.lib",
		"D:/coding/Vulkan/Lib/Vulkan-1"
	}

	libdirs
	{
		"D:/coding/Vulkan/Lib",
		"../Everything/bin/Debug-windows-x86_64/Engine"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		
		
		defines
		{
			PLAT_WINDOWS,
			
		}
	
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Testapp"),
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/UnitTests")
		}
	
	filter "configurations:Debug"
		defines "VK_USE_PLATFORM_WIN32_KHR;VK_PROTOTYPES;RENDERER_DEBUG;VULKAN_DLL"
		symbols "on"
		
	filter "configurations:Release"
		defines "VK_USE_PLATFORM_WIN32_KHR;VK_PROTOTYPES;RENDERER_RELEASE;VULKAN_DLL"
		symbols "on"
		optimize "on"
	
	filter "configurations:Distribution"
		defines "VK_USE_PLATFORM_WIN32_KHR;VK_PROTOTYPES;RENDERER_RELEASE;VULKAN_DLL"
		optimize "on"