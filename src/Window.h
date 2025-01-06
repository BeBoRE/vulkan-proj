#include "VulkanFunctions.h"

struct WindowParameters {
    Display * display;
    Window window;
};

namespace VulkanProject {
  bool CreateDisplay(
    WindowParameters & windowParameters
  );
  bool CreateSurface(
    VkInstance const & vulkanInstance,
    WindowParameters WindowParameters,
    VkSurfaceKHR & surface
  );
  bool CreateWindow(
    VkInstance const & vulkanInstance,
    WindowParameters & windowParameters
  );
  bool DestroyWindow(
    VkInstance const & vulkanInstance,
    WindowParameters & windowParameters,
    VkSurfaceKHR & surface
  );
}
