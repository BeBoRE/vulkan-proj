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

  std::vector<VkExtensionProperties> available_extensions;
  success = VulkanProject::LoadAvailableInstanceExtensions(available_extensions);
  if (!success) return EXIT_FAILURE;

  std::cout << "Available Instance Extensions:\n";
  for (auto extension : available_extensions) {
    std::cout << extension.extensionName << "\n";
  }

  std::vector<const char *> desired_extensions = {
    "bla bla bla, this aint it"
  };

  success = VulkanProject::CheckDesiredExtensions(available_extensions, desired_extensions);
  if (!success) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
