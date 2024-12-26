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
  bool ConnectVulkanLoader( LIBRARY_TYPE & vulkan_library );
  bool LoadExportedVulkanLoaderLibaryFunction( LIBRARY_TYPE const & vulkan_library );
  bool LoadGlobalLevelFunctions();
  bool EnumerateInstanceExtensions( std::vector<VkExtensionProperties> & available_extensions );
  bool CheckDesiredExtensions(
    std::vector<VkExtensionProperties> & available_extensions, 
    std::vector<char const *> const & desired_extensions 
  );
  bool CreateVulkanInstance( 
    VkInstance & vulkan_instance, 
    VkApplicationInfo const & application_info, 
    std::vector<char const *> const & desired_extensions 
  );
  bool LoadInstanceLevelFunctions( 
    VkInstance const & vulkan_instance, 
    std::vector<char const *> const & enabled_extensions
  );
  bool EnumeratePhysicalDevices(
    VkInstance const & vulkan_instance,
    std::vector<VkPhysicalDevice> & available_devices
  );
  bool EnumerateDeviceExtensions ( 
    VkPhysicalDevice physical_device, 
    std::vector<VkExtensionProperties> & available_extensions
  );
  void GetPropertiesPhysicalDevice( 
    VkPhysicalDevice physical_device,
    VkPhysicalDeviceProperties & device_properties 
  );
  bool EnumerateQueueFamilyAndProperties(
    VkPhysicalDevice physical_device,
    std::vector<VkQueueFamilyProperties> & queue_families
  );
  bool IndexOfDesiredQueueFamily(
    VkPhysicalDevice const & physical_device,
    VkQueueFlags const & desired_capabilities,
    uint32_t & queue_family_index
  );
}
