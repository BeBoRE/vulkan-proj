#include "Loading.h"
#include "Window.h"
#include <iostream>
#include <vector>

#include <X11/Xlib.h>

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
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
  };

  VkInstance vulkanInstance;

  success = VulkanProject::CreateVulkanInstance(vulkanInstance, applicationInfo, desiredExtensions);
  if (!success) return EXIT_FAILURE;

  success = VulkanProject::LoadInstanceLevelFunctions(vulkanInstance, desiredExtensions);
  if (!success) return EXIT_FAILURE;

  VkDevice logicalDevice;
  VkQueue graphicsQueue;
  VkQueue computeQueue;

  success = VulkanProject::CreateLogicalDeviceWithComputeGraphics(vulkanInstance, logicalDevice, graphicsQueue, computeQueue);
  if (!success) return EXIT_FAILURE;

  WindowParameters windowParameters;

  success = VulkanProject::CreateDisplay(windowParameters);
  if (!success) return EXIT_FAILURE;

  success = VulkanProject::CreateWindow(vulkanInstance, windowParameters);
  if (!success) return EXIT_FAILURE;

  VkSurfaceKHR surface;

  success = VulkanProject::CreateSurface(vulkanInstance, windowParameters, surface);
  if (!success) return EXIT_FAILURE;

  VulkanProject::DestroyWindow(vulkanInstance, windowParameters, surface);

  VulkanProject::DestroyLogicalDevice(logicalDevice);
  VulkanProject::DestroyVulkanInstance(vulkanInstance);
  VulkanProject::CloseVulkanLoader(vulkanLibraryLoader);

  return EXIT_SUCCESS;
}
