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

include "Kame/vendor/GLFW"
include "Kame/vendor/Glad"
include "Kame/vendor/imgui"
include "Kame/vendor/DirectXTex/DirectXTex"

project "Kame"
  location "Kame"
  kind "SharedLib"
  language "C++"

  targetdir ("bin/" .. outputdir  .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir  .. "/%{prj.name}")

  pchheader "kmpch.h"
  pchsource "Kame/src/kmpch.cpp"

  files {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
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
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    defines {
      "KAME_PLATFORM_WINDOWS",
      "KAME_BUILD_DLL",
      "GLFW_INCLUDE_NONE",
	  "KAME_PLATFORM_WIN32",
	  "KAME_PLATFORM_DIRECTX12"
    }

    postbuildcommands {
      ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
    }

  filter "configurations:Debug"
    defines "KAME_DEBUG"
    buildoptions "/MDd"
    symbols "On"

  filter "configurations:Release"
    defines "KAME_RELEASE"
    buildoptions "/MD"
    optimize "On"

  filter "configurations:Dist"
    defines "KAME_DIST"
    buildoptions "/MD"
    optimize "On"
  
project "Sandbox"
  location "Sandbox"
  kind "ConsoleApp"
  language "C++"

  targetdir ("bin/" .. outputdir  .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir  .. "/%{prj.name}")

  files {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
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
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    defines {
      "KAME_PLATFORM_WINDOWS"
    }

  filter "configurations:Debug"
    defines "KAME_DEBUG"
    buildoptions "/MDd"
    symbols "On"

  filter "configurations:Release"
    defines "KAME_RELEASE"
    buildoptions "/MD"
    optimize "On"

  filter "configurations:Dist"
    defines "KAME_DIST"
    buildoptions "/MD"
    optimize "On"