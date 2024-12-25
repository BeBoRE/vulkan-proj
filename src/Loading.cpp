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

  bool EnumerateAvailableInstanceExtensions( std::vector<VkExtensionProperties> & available_extensions ) {
    uint32_t extensions_count = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);

    if (result != VK_SUCCESS) {
      std::cout << "Couldn't retrieve extension count\n";
      return false;
    }
    
    available_extensions.resize(extensions_count);

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());
    if (result != VK_SUCCESS) {
      std::cout << "Couldn't enumerate instance extensions\n";
      return false;
    };

    return true;
  }

  bool CheckDesiredExtensions( std::vector<VkExtensionProperties> & available_extensions, std::vector<char const *> const & desired_extensions ) {
    std::vector<std::string> not_found_extensions;
    
    for (auto desired : desired_extensions) {
      bool found = false;
      for (auto available : available_extensions) {
        if (!std::strcmp(available.extensionName, desired)) {
          found = true;
          break;
        };
      }

      if (!found) {
        not_found_extensions.push_back(desired);
      }
    }

    if (not_found_extensions.size()) {
      std::cout << "Could not find extension(s):\n";
      for(auto extension : not_found_extensions) {
        std::cout << "\t" << extension << "\n";
      }
      std::cout << "\n";

      std::cout << "Available extensions:" << "\n";
      for(auto available : available_extensions) {
        std::cout << "\t" << available.extensionName << "\n";
      }

      return false;
    }

    return true;
  }

  bool CreateVulkanInstance( VkInstance & vulkan_instance, VkApplicationInfo const & application_info, std::vector<const char *> const & desired_extensions ) {
    std::vector<VkExtensionProperties> available_extensions;
    if (!EnumerateAvailableInstanceExtensions(available_extensions)) {
      return false;
    }

    if (!CheckDesiredExtensions(available_extensions, desired_extensions)) {
      return false;
    }

    VkInstanceCreateInfo instance_create_info = {
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      nullptr,
      0,
      &application_info,
      0,
      nullptr,
      static_cast<uint32_t>(desired_extensions.size()),
      desired_extensions.data()
    };

    VkResult result = vkCreateInstance( &instance_create_info, nullptr, &vulkan_instance);

    if (result != VK_SUCCESS || vulkan_instance == VK_NULL_HANDLE) {
      std::cout << "Could not create Vulkan instance\n";
      return false;
    }

    return true;
  }

  bool LoadInstanceLevelFunctions( VkInstance const & vulkan_instance, std::vector<char const *> const & enabled_extensions) {
    #define INSTANCE_LEVEL_VULKAN_FUNCTION( name )                                \
      name = (PFN_##name)vkGetInstanceProcAddr( vulkan_instance, #name );         \
      if( name == nullptr ) {                                                     \
        std::cout << "Could not load instance-level Vulkan function named: "      \
          #name << std::endl;                                                     \
        return false;                                                             \
      }

    // Load instance-level functions from enabled extensions
    #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )      \
      for( auto & enabled_extension : enabled_extensions ) {                      \
        if( std::string( enabled_extension ) == std::string( extension ) ) {      \
          name = (PFN_##name)vkGetInstanceProcAddr( vulkan_instance, #name );     \
          if( name == nullptr ) {                                                 \
            std::cout << "Could not load instance-level Vulkan function named: "  \
              #name << std::endl;                                                 \
            return false;                                                         \
          }                                                                       \
        }                                                                         \
      }

    #include "ListOfVulkanFunctions.inl"

    return true;
  }

  bool EnumerateAvailablePhysicalDevices(
    VkInstance const & vulkan_instance,
    std::vector<VkPhysicalDevice> & available_devices
  ) {
    uint32_t devices_count = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumeratePhysicalDevices(vulkan_instance, &devices_count, nullptr);
    if (result != VK_SUCCESS) {
      std::cout << "Could not get number of available physical devices\n";
      return false;
    }

    if (!devices_count) {
      std::cout << "No available physical devices\n";
      return false;
    }

    available_devices.resize(devices_count);
    result = vkEnumeratePhysicalDevices(vulkan_instance, &devices_count, available_devices.data());

    if (result != VK_SUCCESS) {
      std::cout << "Could not enumerate physical devices\n";
      return false;
    }

    return true;
  }

  bool EnumerateAvailableDeviceExtensions ( 
    VkPhysicalDevice physical_device, 
    std::vector<VkExtensionProperties> & available_extensions
  ) {
    uint32_t extension_count = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &extension_count, nullptr );
    if (result != VK_SUCCESS) {
      std::cout << "Could not get the number of device extensions\n";

      return false;
    }

    available_extensions.resize(extension_count);
    result = vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, 
      &extension_count, 
      available_extensions.data()
    );

    if (result != VK_SUCCESS) {
      std::cout << "Could not enumerate device extensions\n";
      return false;
    }

    return true;
  }

  void GetFeaturesAndPropertiesOfPhysicalDevice( 
    VkPhysicalDevice physical_device,
    VkPhysicalDeviceFeatures & device_features,
    VkPhysicalDeviceProperties & device_properties 
  ) {
    vkGetPhysicalDeviceFeatures( physical_device, &device_features );

    vkGetPhysicalDeviceProperties( physical_device, &device_properties );
  };
} // namespace VulkanProject


