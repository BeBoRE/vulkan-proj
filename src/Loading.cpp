#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <cstring>
#include "Loading.h"

namespace VulkanProject {
  bool ConnectVulkanLoader( LIBRARY_TYPE & vulkan_library ) {
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

  bool LoadExportedVulkanLoaderLibaryFunction(LIBRARY_TYPE const & vulkan_library) {
    #define EXPORTED_VULKAN_FUNCTION( name )                              \
    name = (PFN_##name)LoadFunction( vulkan_library, #name );         \
    if( name == nullptr ) {                                           \
      std::cout << "Could not load exported Vulkan function named: "  \
        #name << std::endl;                                           \
      return false;                                                   \
    }

    #include "ListOfVulkanFunctions.inl"

    return true;
  }

  bool LoadGlobalLevelFunctions() {
    #define GLOBAL_LEVEL_VULKAN_FUNCTION( name )                            \
      name = (PFN_##name)vkGetInstanceProcAddr( nullptr, #name );           \
      if( name == nullptr ) {                                               \
        std::cout << "Could not load global level Vulkan function named: "  \
          #name << std::endl;                                               \
        return false;                                                       \
      }

    #include "ListOfVulkanFunctions.inl"

    return true;
  }

  bool LoadAvailableInstanceExtensions( std::vector<VkExtensionProperties> & available_extensions ) {
    uint32_t extensions_count = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);

    if (result != VK_SUCCESS || extensions_count == 0) {
      std::cout << "Couldn't retrieve extension count\n";
      return false;
    }
    
    available_extensions.resize(extensions_count);

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, &available_extensions.front());
    if (result != VK_SUCCESS) {
      std::cout << "Couldn't enumerate instance extensions\n";
      return false;
    };

    return true;
  }

  bool CheckDesiredExtensions( std::vector<VkExtensionProperties> & available_extensions, std::vector<char const *> & desired_extensions ) {
    for (auto desired : desired_extensions) {
      for (auto available : available_extensions) {
        if (!std::strcmp(available.extensionName, desired)) {
          break;
        };

        std::cout << "Could not find extension " << desired << std::endl;

        return false;
      }
    }

    return true;
  }
} // namespace VulkanProject


