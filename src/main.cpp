#include "Loading.h"
#include <iostream>
#include <vector>

int main()
{
  LIBRARY_TYPE vulkanLibraryLoader;

  bool success;

  success = VulkanProject::ConnectVulkanLoader(vulkanLibraryLoader);
  if (!success) return EXIT_FAILURE;

  success = VulkanProject::LoadExportedVulkanLoaderLibaryFunction(vulkanLibraryLoader);
  if (!success) return EXIT_FAILURE;

  success = VulkanProject::LoadGlobalLevelFunctions();
  if (!success) return EXIT_FAILURE;

  VkApplicationInfo applicationInfo = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO,
    nullptr,
    "Vulkan thing",
    VK_MAKE_VERSION(1, 0, 0),
    "My own :D",
    VK_MAKE_VERSION(1, 0, 0),
    VK_MAKE_VERSION(1, 0, 0)
  };

  std::vector<const char *> desiredExtensions = {
    "VK_KHR_xlib_surface"
  };

  VkInstance vulkanInstance;

  success = VulkanProject::CreateVulkanInstance(vulkanInstance, applicationInfo, desiredExtensions);
  if (!success) return EXIT_FAILURE;

  success = VulkanProject::LoadInstanceLevelFunctions(vulkanInstance, desiredExtensions);
  if (!success) return EXIT_FAILURE;

  std::vector<VkPhysicalDevice> availableDevices;
  success = VulkanProject::EnumeratePhysicalDevices(vulkanInstance, availableDevices);
  if (!success) return EXIT_FAILURE;

  if(!availableDevices.size()) {
    return EXIT_FAILURE;
  }

  std::cout << availableDevices.size() << " physical devices available:\n";
  for (u_int32_t pd_index = 0; pd_index < static_cast<u_int32_t>(availableDevices.size()); pd_index = pd_index + 1) {
    auto pd = availableDevices[pd_index];
    VkPhysicalDeviceProperties properties;
    VulkanProject::GetPropertiesPhysicalDevice(pd, properties);

    std::cout << "\t" << pd_index << ": " << properties.deviceName << "\n";

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    bool success = VulkanProject::EnumerateQueueFamilyAndProperties(pd, queueFamilyProperties);

    std::cout << "\t\t" << queueFamilyProperties.size() << " queue families available:\n";
    for (
      u_int32_t qfpIndex = 0;
      qfpIndex < static_cast<u_int32_t>(queueFamilyProperties.size());
      qfpIndex = qfpIndex + 1
      ) {
      auto qfp = queueFamilyProperties[qfpIndex];
      std::cout << "\t\t\t" << qfpIndex << ": ";

      if (qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT) std::cout << "G ";
      if (qfp.queueFlags & VK_QUEUE_COMPUTE_BIT) std::cout << "C ";
      if (qfp.queueFlags & VK_QUEUE_TRANSFER_BIT) std::cout << "T ";
      if (qfp.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) std::cout << "S ";
      if (qfp.queueFlags & VK_QUEUE_PROTECTED_BIT) std::cout << "P ";

      std::cout << "(" << qfp.queueCount << " available)" "\n";
    }
  }

  auto physical_device = availableDevices[0];
  u_int32_t queue_family_index;

  success = VulkanProject::IndexOfDesiredQueueFamily(physical_device, VK_QUEUE_GRAPHICS_BIT, queue_family_index);
  if (!success) return EXIT_FAILURE;

  std::cout << "Selected index " << queue_family_index << "\n";

  return EXIT_SUCCESS;
}
