workspace "Kame"
  architecture "x64"

  configurations {
    "Debug",
    "Release",
    "Dist"
  }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Kame/vendor/GLFW/include"
IncludeDir["Glad"] = "Kame/vendor/GLAD/include"
IncludeDir["ImGui"] = "Kame/vendor/imgui"
IncludeDir["glm"] = "Kame/vendor/glm"
IncludeDir["DirectXTex"] = "Kame/vendor/DirectXTex/DirectXTex"
IncludeDir["VulkanSdk"] = "$(VULKAN_SDK)/include"

include "Kame/vendor/GLFW"
include "Kame/vendor/Glad"
include "Kame/vendor/imgui"
include "Kame/vendor/DirectXTex/DirectXTex"

project "Kame"
  location "Kame"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"

  targetdir ("bin/" .. outputdir  .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir  .. "/%{prj.name}")

  pchheader "kmpch.h"
  pchsource "Kame/src/kmpch.cpp"

  files {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
    "%{prj.name}/src/**.hlsl"
  }

  includedirs {
    "%{prj.name}/src",
    "%{prj.name}/vendor/spdlog/include",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.Glad}",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.DirectXTex}"
  }

  links {
    "GLFW",
    "Glad",
    "ImGui",
    "DirectXTex",
    "opengl32.lib"
  }

  filter "system:windows"
    systemversion "latest"

    defines {
      "KAME_PLATFORM_WINDOWS",
      "KAME_BUILD_DLL",
      "GLFW_INCLUDE_NONE",
	    "KAME_PLATFORM_WIN32",
	    "KAME_PLATFORM_DIRECTX12"
    }

  filter "configurations:Debug"
    defines "KAME_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "KAME_RELEASE"
    runtime "Release"
    optimize "on"

  filter "configurations:Dist"
    defines "KAME_DIST"
    runtime "Release"
    optimize "on"

  filter { "files:**.hlsl" }
    shadermodel "6.0"
    shadervariablename "g_%{file.basename}"
    shaderheaderfileoutput "D:/Raftek/Kame/Kame/src/Kame/Platform/DirectX12/Graphics/%{file.basename}.h"
    shaderobjectfileoutput "D:/Raftek/Kame/Kame/src/Kame/Platform/DirectX12/Graphics/%{file.basename}.cso"
  filter { "files:**_PS.hlsl" }
    shadertype "Pixel"
  filter { "files:**_CS.hlsl" }
    shadertype "Compute"
  filter { "files:**_VS.hlsl" }
    shadertype "Vertex"

project "Sandbox"
  location "Sandbox"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"

  targetdir ("bin/" .. outputdir  .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir  .. "/%{prj.name}")

  files {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
    "%{prj.name}/src/**.hlsl"
  }

  includedirs {
    "Kame/vendor/spdlog/include",
    "Kame/src",
    "%{IncludeDir.glm}"
  }

  links {
    "Kame"
  }

  filter "system:windows"
    systemversion "latest"

    defines {
      "KAME_PLATFORM_WINDOWS"
    }

  filter "configurations:Debug"
    defines "KAME_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "KAME_RELEASE"
    runtime "Release"
    optimize "on"

  filter "configurations:Dist"
    defines "KAME_DIST"
    runtime "Release"
    optimize "on"

  filter { "files:**.hlsl" }
    shadermodel "6.0"
    shadervariablename "g_%{file.basename}"
    shaderheaderfileoutput "D:/Raftek/Kame/Sandbox/src/%{file.basename}.h"
  filter { "files:**_PS.hlsl" }
    shadertype "Pixel"
  filter { "files:**_CS.hlsl" }
    shadertype "Compute"
  filter { "files:**_VS.hlsl" }
    shadertype "Vertex"
