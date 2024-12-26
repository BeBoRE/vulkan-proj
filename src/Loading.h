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
  bool LoadDeviceLevelFunctions(
    VkDevice logicalDevice,
    std::vector<char const *> const & enabledExtensions
  );
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
  bool EnumerateDeviceExtensions( 
    VkPhysicalDevice physicalDevice, 
    std::vector<VkExtensionProperties> & availableExtensions
  );
  void GetPropertiesPhysicalDevice( 
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceProperties & deviceProperties 
  );
  void GetFeaturesPhysicalDevice( 
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceFeatures & deviceFeatures
  );
  bool EnumerateQueueFamilyAndProperties(
    VkPhysicalDevice physicalDevice,
    std::vector<VkQueueFamilyProperties> & queueFamilies
  );
  bool IndexOfQueueFamilyWith(
    VkPhysicalDevice const & physicalDevice,
    VkQueueFlags const & desiredCapabilities,
    uint32_t & queueFamilyIndex
  );
  struct QueueInfo {
    uint32_t index;
    std::vector<float> priorities;
  };

  bool CreateLogicalDevice(
    VkPhysicalDevice physicalDevice,
    std::vector<QueueInfo> queueInfos,
    std::vector<char const *> desiredExtensions,
    VkPhysicalDeviceFeatures * desiredFeatures,
    VkDevice & logicalDevice
  );
  void GetDeviceQueue(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue & queue);
  bool CreateLogicalDeviceWithComputeGraphics(
    VkInstance instance,
    VkDevice & logicalDevice,
    VkQueue & graphicsQueue,
    VkQueue & computeQueue
  );
  void DestroyVulkanInstance( VkInstance & vulkanInstance );
  void DestroyLogicalDevice( VkDevice & logicalDevice );
  void CloseVulkanLoader( LIBRARY_TYPE & vulkanLibrary );
}
