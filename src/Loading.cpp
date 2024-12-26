#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <cstring>
#include "Loading.h"

namespace VulkanProject {
  bool ConnectVulkanLoader( LIBRARY_TYPE & vulkanLibrary ) {
    #if defined _WIN32
        vulkanLibrary = LoadLibrary( "vulkan-1.dll" );
    #elif defined __linux
        vulkanLibrary = dlopen( "libvulkan.so.1", RTLD_NOW );
    #endif

    if( vulkanLibrary == nullptr ) {
      std::cout << "Could not connect with a Vulkan Runtime library." << std::endl;
      return false;
    }
    return true;
  }

  bool LoadExportedVulkanLoaderLibaryFunction(LIBRARY_TYPE const & vulkanLibrary) {
    #define EXPORTED_VULKAN_FUNCTION( name )                              \
    name = (PFN_##name)LoadFunction( vulkanLibrary, #name );         \
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

  bool EnumerateInstanceExtensions( std::vector<VkExtensionProperties> & availableExtensions ) {
    uint32_t extensionCount = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    if (result != VK_SUCCESS) {
      std::cout << "Couldn't retrieve extension count\n";
      return false;
    }
    
    availableExtensions.resize(extensionCount);

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
    if (result != VK_SUCCESS) {
      std::cout << "Couldn't enumerate instance extensions\n";
      return false;
    };

    return true;
  }

  bool CheckDesiredExtensions( std::vector<VkExtensionProperties> & availableExtensions, std::vector<char const *> const & desiredExtensions ) {
    std::vector<std::string> notFoundExtensions;
    
    for (auto desired : desiredExtensions) {
      bool found = false;
      for (auto available : availableExtensions) {
        if (!std::strcmp(available.extensionName, desired)) {
          found = true;
          break;
        };
      }

      if (!found) {
        notFoundExtensions.push_back(desired);
      }
    }

    if (notFoundExtensions.size()) {
      std::cout << "Could not find extension(s):\n";
      for(auto extension : notFoundExtensions) {
        std::cout << "\t" << extension << "\n";
      }
      std::cout << "\n";

      std::cout << "Available extensions:" << "\n";
      for(auto available : availableExtensions) {
        std::cout << "\t" << available.extensionName << "\n";
      }

      return false;
    }

    return true;
  }

  bool CreateVulkanInstance(
    VkInstance & vulkanInstance, 
    VkApplicationInfo const & applicationInfo, 
    std::vector<const char *> const & desiredExtensions 
  ) {
    std::vector<VkExtensionProperties> availableExtensions;
    if (!EnumerateInstanceExtensions(availableExtensions)) {
      return false;
    }

    if (!CheckDesiredExtensions(availableExtensions, desiredExtensions)) {
      return false;
    }

    VkInstanceCreateInfo instanceCreateInfo = {
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      nullptr,
      0,
      &applicationInfo,
      0,
      nullptr,
      static_cast<uint32_t>(desiredExtensions.size()),
      desiredExtensions.data()
    };

    VkResult result = vkCreateInstance( &instanceCreateInfo, nullptr, &vulkanInstance);

    if (result != VK_SUCCESS || vulkanInstance == VK_NULL_HANDLE) {
      std::cout << "Could not create Vulkan instance\n";
      return false;
    }

    return true;
  }

  bool LoadInstanceLevelFunctions( VkInstance const & vulkanInstance, std::vector<char const *> const & enabledExtensions) {
    #define INSTANCE_LEVEL_VULKAN_FUNCTION( name )                                \
      name = (PFN_##name)vkGetInstanceProcAddr( vulkanInstance, #name );         \
      if( name == nullptr ) {                                                     \
        std::cout << "Could not load instance-level Vulkan function named: "      \
          #name << std::endl;                                                     \
        return false;                                                             \
      }

    // Load instance-level functions from enabled extensions
    #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )      \
      for( auto & enabled_extension : enabledExtensions ) {                      \
        if( std::string( enabled_extension ) == std::string( extension ) ) {      \
          name = (PFN_##name)vkGetInstanceProcAddr( vulkanInstance, #name );     \
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

  bool EnumeratePhysicalDevices(
    VkInstance const & vulkanInstance,
    std::vector<VkPhysicalDevice> & availableDevices
  ) {
    uint32_t devicesCount = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumeratePhysicalDevices(vulkanInstance, &devicesCount, nullptr);
    if (result != VK_SUCCESS) {
      std::cout << "Could not get number of available physical devices\n";
      return false;
    }

    if (!devicesCount) {
      std::cout << "No available physical devices\n";
      return false;
    }

    availableDevices.resize(devicesCount);
    result = vkEnumeratePhysicalDevices(vulkanInstance, &devicesCount, availableDevices.data());

    if (result != VK_SUCCESS) {
      std::cout << "Could not enumerate physical devices\n";
      return false;
    }

    return true;
  }

  bool EnumerateDeviceExtensions ( 
    VkPhysicalDevice physical_device, 
    std::vector<VkExtensionProperties> & availableExtensions
  ) {
    uint32_t extension_count = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &extension_count, nullptr );
    if (result != VK_SUCCESS) {
      std::cout << "Could not get the number of device extensions\n";

      return false;
    }

    availableExtensions.resize(extension_count);
    result = vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, 
      &extension_count, 
      availableExtensions.data()
    );

    if (result != VK_SUCCESS) {
      std::cout << "Could not enumerate device extensions\n";
      return false;
    }

    return true;
  }

  void GetPropertiesPhysicalDevice( 
    VkPhysicalDevice physical_device,
    VkPhysicalDeviceProperties & device_properties 
  ) {
    vkGetPhysicalDeviceProperties( physical_device, &device_properties );
  };

  bool EnumerateQueueFamilyAndProperties(
    VkPhysicalDevice physical_device,
    std::vector<VkQueueFamilyProperties> & queue_families
  ) {
    uint32_t queue_families_count = 0;

    vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &queue_families_count, nullptr );
    if( queue_families_count == 0 ) {
      std::cout << "Could not get the number of queue families\n";
      return false;
    }

    queue_families.resize( queue_families_count );
    vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &queue_families_count, queue_families.data() );
    if( queue_families_count == 0 ) {
      std::cout << "Could not acquire properties of queue families\n";
      return false;
    }

    return true;
  }

  bool IndexOfDesiredQueueFamily(
    VkPhysicalDevice const & physical_device,
    VkQueueFlags const & desired_capabilities,
    uint32_t & queue_family_index
  ) {
    std::vector<VkQueueFamilyProperties> queue_family_properties;
    if (!VulkanProject::EnumerateQueueFamilyAndProperties(physical_device, queue_family_properties)) {
      return false;
    }

    for (u_int32_t index = 0; index < static_cast<u_int8_t>(queue_family_properties.size()); index = index + 1) {
      auto queue_family = queue_family_properties[index];
      if (queue_family.queueFlags & desired_capabilities == desired_capabilities) {
        queue_family_index = index;
        return true;
      }
    }

    return false;
  }
} // namespace VulkanProject


