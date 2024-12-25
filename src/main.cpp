#include "Loading.h"
#include <iostream>
#include <vector>

int main()
{
  LIBRARY_TYPE vulkan_library_loader;

  bool success;

  success = VulkanProject::ConnectVulkanLoader(vulkan_library_loader);
  if (!success) return EXIT_FAILURE;

  success = VulkanProject::LoadExportedVulkanLoaderLibaryFunction(vulkan_library_loader);
  if (!success) return EXIT_FAILURE;

  success = VulkanProject::LoadGlobalLevelFunctions();
  if (!success) return EXIT_FAILURE;

  VkApplicationInfo application_info = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO,
    nullptr,
    "Vulkan thing",
    VK_MAKE_VERSION(1, 0, 0),
    "My own :D",
    VK_MAKE_VERSION(1, 0, 0),
    VK_MAKE_VERSION(1, 0, 0)
  };

  std::vector<const char *> desired_extensions = {
    "VK_KHR_xlib_surface"
  };

  VkInstance vulkan_instance;

  success = VulkanProject::CreateVulkanInstance(vulkan_instance, application_info, desired_extensions);
  if (!success) return EXIT_FAILURE;

  success = VulkanProject::LoadInstanceLevelFunctions(vulkan_instance, desired_extensions);
  if (!success) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
