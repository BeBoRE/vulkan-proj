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
    #define EXPORTED_VULKAN_FUNCTION( name )                          \
    name = (PFN_##name)LoadFunction( vulkanLibrary, #name );          \
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
      name = (PFN_##name)vkGetInstanceProcAddr( vulkanInstance, #name );          \
      if( name == nullptr ) {                                                     \
        std::cout << "Could not load instance-level Vulkan function named: "      \
          #name << std::endl;                                                     \
        return false;                                                             \
      }

    // Load instance-level functions from enabled extensions
    #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )      \
      for( auto & enabled_extension : enabledExtensions ) {                       \
        if( std::string( enabled_extension ) == std::string( extension ) ) {      \
          name = (PFN_##name)vkGetInstanceProcAddr( vulkanInstance, #name );      \
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

  bool LoadDeviceLevelFunctions(
    VkDevice logicalDevice,
    std::vector<char const *> const & enabledExtensions
  ) {
    #define DEVICE_LEVEL_VULKAN_FUNCTION( name )                                  \
      name = (PFN_##name)vkGetDeviceProcAddr( logicalDevice, #name );             \
      if( name == nullptr ) {                                                     \
        std::cout << "Could not load device-level Vulkan function named: "        \
          #name << std::endl;                                                     \
        return false;                                                             \
      }

    #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )        \
      for( auto & enabledExtension : enabledExtensions ) {                        \
        if( std::string( enabledExtension ) == std::string( extension ) ) {       \
          name = (PFN_##name)vkGetDeviceProcAddr( logicalDevice, #name );         \
          if( name == nullptr ) {                                                 \
            std::cout << "Could not load device-level Vulkan function named: "    \
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
    VkPhysicalDevice physicalDevice, 
    std::vector<VkExtensionProperties> & availableExtensions
  ) {
    uint32_t extensionCount = 0;
    VkResult result = VK_SUCCESS;

    result = vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, nullptr );
    if (result != VK_SUCCESS) {
      std::cout << "Could not get the number of device extensions\n";

      return false;
    }

    availableExtensions.resize(extensionCount);
    result = vkEnumerateDeviceExtensionProperties(
      physicalDevice, nullptr, 
      &extensionCount, 
      availableExtensions.data()
    );

    if (result != VK_SUCCESS) {
      std::cout << "Could not enumerate device extensions\n";
      return false;
    }

    return true;
  }

  void GetPropertiesPhysicalDevice( 
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceProperties & deviceProperties 
  ) {
    vkGetPhysicalDeviceProperties( physicalDevice, &deviceProperties );
  };

  bool EnumerateQueueFamilyAndProperties(
    VkPhysicalDevice physicalDevice,
    std::vector<VkQueueFamilyProperties> & queueFamilies
  ) {
    uint32_t queueFamiliesCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamiliesCount, nullptr );
    if( queueFamiliesCount == 0 ) {
      std::cout << "Could not get the number of queue families\n";
      return false;
    }

    queueFamilies.resize( queueFamiliesCount );
    vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamiliesCount, queueFamilies.data() );
    if( queueFamiliesCount == 0 ) {
      std::cout << "Could not acquire properties of queue families\n";
      return false;
    }

    return true;
  }

  void GetFeaturesPhysicalDevice( 
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceFeatures & deviceFeatures
  ) {
    vkGetPhysicalDeviceFeatures( physicalDevice, &deviceFeatures );
  }

  bool IndexOfQueueFamilyWith(
    VkPhysicalDevice const & physicalDevice,
    VkQueueFlags const & desiredCapabilities,
    uint32_t & queueFamilyIndex
  ) {
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    if (!VulkanProject::EnumerateQueueFamilyAndProperties(physicalDevice, queueFamilyProperties)) {
      return false;
    }

    for (u_int32_t index = 0; index < static_cast<u_int8_t>(queueFamilyProperties.size()); index = index + 1) {
      auto queue_family = queueFamilyProperties[index];
      if (queue_family.queueFlags & desiredCapabilities == desiredCapabilities) {
        queueFamilyIndex = index;
        return true;
      }
    }

    return false;
  }

  bool CreateLogicalDevice(
    VkPhysicalDevice physicalDevice,
    std::vector<QueueInfo> queueInfos,
    std::vector<char const *> desiredExtensions,
    VkPhysicalDeviceFeatures * desiredFeatures,
    VkDevice & logicalDevice
  ) {
    std::vector<VkExtensionProperties> availableExtensions;
    if (!EnumerateDeviceExtensions(physicalDevice, availableExtensions)) {
      return false;
    }

    if (!CheckDesiredExtensions(availableExtensions, desiredExtensions)) {
      return false;
    }
  
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for (auto & info : queueInfos) {
      queueCreateInfos.push_back({
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,
        0,
        info.index,
        static_cast<uint32_t>(info.priorities.size()),
        info.priorities.data()
      });
    }

    VkDeviceCreateInfo deviceCreateInfo = {
      VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      nullptr,
      0,
      static_cast<uint32_t>(queueCreateInfos.size()),
      queueCreateInfos.data(),
      0,
      nullptr,
      static_cast<uint32_t>(desiredExtensions.size()),
      desiredExtensions.data()
    };

    VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
    if(result != VK_SUCCESS || logicalDevice == VK_NULL_HANDLE) {
      std::cout << "Unable to create logical device\n";
      return false;
    }

    return true;
  }

  void GetDeviceQueue(
    VkDevice logicalDevice, 
    uint32_t queueFamilyIndex, 
    uint32_t queueIndex, 
    VkQueue & queue
  ) {
    vkGetDeviceQueue(logicalDevice, queueFamilyIndex, queueIndex, &queue);
  }

  bool CreateLogicalDeviceWithComputeGraphics(
    VkInstance instance,
    VkDevice & logicalDevice,
    VkQueue & graphicsQueue,
    VkQueue & computeQueue
  ) {
    std::vector<VkPhysicalDevice> physicalDevices;
    if (!EnumeratePhysicalDevices(instance, physicalDevices)) {
      return false;
    }

    for (auto & physicalDevice : physicalDevices) {
      VkPhysicalDeviceFeatures deviceFeatures;
      GetFeaturesPhysicalDevice(physicalDevice, deviceFeatures);

      VkPhysicalDeviceProperties deviceProperties;
      GetPropertiesPhysicalDevice(physicalDevice, deviceProperties);

      if (!deviceFeatures.geometryShader) {
        continue;
      }

      uint32_t graphicsQueueFamilyIndex;
      if (!IndexOfQueueFamilyWith(physicalDevice, VK_QUEUE_GRAPHICS_BIT, graphicsQueueFamilyIndex)) {
        continue;
      }

      uint32_t computeQueueFamilyIndex;
      if (!IndexOfQueueFamilyWith(physicalDevice, VK_QUEUE_COMPUTE_BIT, computeQueueFamilyIndex)) {
        continue;
      }

      std::vector<QueueInfo> queueInfos = {
        { graphicsQueueFamilyIndex, { 1.0f } }
      };

      if (graphicsQueueFamilyIndex != computeQueueFamilyIndex) {
        queueInfos.push_back({ computeQueueFamilyIndex, { 1.0f } });
      }

      if (!CreateLogicalDevice(physicalDevice, queueInfos, {}, &deviceFeatures, logicalDevice)) {
        continue;
      } else {
        if (!LoadDeviceLevelFunctions(logicalDevice, {})) {
          return false;
        }

        GetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, graphicsQueue);
        GetDeviceQueue(logicalDevice, computeQueueFamilyIndex, 0, computeQueue);
        return true;
      }
    }

    return false;
  }

  void DestroyVulkanInstance( VkInstance & vulkanInstance ) {
    vkDestroyInstance(vulkanInstance, nullptr);
    vulkanInstance = VK_NULL_HANDLE;
  }

  void DestroyLogicalDevice( VkDevice & logicalDevice ) {
    vkDestroyDevice(logicalDevice, nullptr);
    logicalDevice = VK_NULL_HANDLE;
  }

  void CloseVulkanLoader( LIBRARY_TYPE & vulkanLibrary ) {
    #if defined _WIN32
      FreeLibrary( vulkanLibrary );
    #elif defined __linux
      dlclose( vulkanLibrary );
    #endif

    vulkanLibrary = nullptr;
  }
} // namespace VulkanProject


