workspace "Vulkan Project"
   configurations { "Debug", "Release" }

project "HelloWorld"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"

   targetdir ".build/%{cfg.buildcfg}/bin"
   objdir ".build/${cfg.buildcfg}/obj"

   location "./src"
   files { "%{prj.location}/**.h", "%{prj.location}/**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
