#include "ConnectVulkanLoader.h"

int main()
{
  LIBRARY_TYPE vulkan_library_loader;
  VulkanProject::ConnectVulkanLoader(vulkan_library_loader);

  return 0;
}
