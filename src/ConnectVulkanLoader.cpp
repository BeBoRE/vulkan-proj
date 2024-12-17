#include <iostream>
#include <dlfcn.h>
#include "ConnectVulkanLoader.h"

namespace VulkanProject {
  bool ConnectVulkanLoader( LIBRARY_TYPE vulkan_library ) {
#if defined _WIN32
    vulkan_library = LoadLibrary( "vulkan-1.dll" );
#elif defined __linux
    vulkan_library = dlopen( "libvulkan.so.1", RTLD_NOW );
#endif

    if( vulkan_library == nullptr ) {
      std::cout << "Could not connect with a Vulkan Runtime library." << std::endl;
      return false;
    }
    return true;
  }
} // namespace VulkanProject
