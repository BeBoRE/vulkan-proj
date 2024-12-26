#include <vector>
#include "VulkanFunctions.h"

#if defined _WIN32
  typedef HMODULE LIBRARY_TYPE;
  #define LoadFunction GetProcAddress
#elif defined __linux
  typedef void* LIBRARY_TYPE;
  #define LoadFunction dlsym
#endif

namespace VulkanProject {
  bool ConnectVulkanLoader( LIBRARY_TYPE & vulkanLibrary );
  bool LoadExportedVulkanLoaderLibaryFunction( LIBRARY_TYPE const & vulkanLibrary );
  bool LoadGlobalLevelFunctions();
  bool EnumerateInstanceExtensions( std::vector<VkExtensionProperties> & availableExtensions );
  bool CheckDesiredExtensions(
    std::vector<VkExtensionProperties> & availableExtensions, 
    std::vector<char const *> const & desiredExtensions 
  );
  bool CreateVulkanInstance( 
    VkInstance & vulkanInstance, 
    VkApplicationInfo const & applicationInfo, 
    std::vector<char const *> const & desiredExtensions 
  );
  bool LoadInstanceLevelFunctions( 
    VkInstance const & vulkanInstance, 
    std::vector<char const *> const & enabledExtensions
  );
  bool EnumeratePhysicalDevices(
    VkInstance const & vulkanInstance,
    std::vector<VkPhysicalDevice> & availableDevices
  );
  bool EnumerateDeviceExtensions ( 
    VkPhysicalDevice physicalDevice, 
    std::vector<VkExtensionProperties> & availableExtensions
  );
  void GetPropertiesPhysicalDevice( 
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceProperties & deviceProperties 
  );
  bool EnumerateQueueFamilyAndProperties(
    VkPhysicalDevice physicalDevice,
    std::vector<VkQueueFamilyProperties> & queueFamilies
  );
  bool IndexOfDesiredQueueFamily(
    VkPhysicalDevice const & physicalDevice,
    VkQueueFlags const & desiredCapabilities,
    uint32_t & queueFamilyIndex
  );
}
