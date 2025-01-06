#include "Window.h"
#include <iostream>

namespace VulkanProject {
  bool CreateDisplay(
    WindowParameters & windowParameters
  ) {
    windowParameters.display = XOpenDisplay(nullptr);
    if (windowParameters.display == nullptr) {
      std::cout << "Unable to open X display\n";
      return false;
    }

    return true;
  };

  bool CreateSurface(
    VkInstance const & vulkanInstance,
    WindowParameters windowParameters,
    VkSurfaceKHR & surface
  ) {
    VkResult result;

    VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {
      VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
      nullptr,
      0,
      windowParameters.display,
      windowParameters.window
    };

    result = vkCreateXlibSurfaceKHR(vulkanInstance, &surfaceCreateInfo, nullptr, &surface);

    if( result != VK_SUCCESS ) {
      std::cout << "Unable to create presentation surface\n";
      return false;
    }

    return true;
  }

  bool CreateWindow(
    VkInstance const & vulkanInstance,
    WindowParameters & windowParameters
  ) {
    windowParameters.window = XCreateSimpleWindow(
      windowParameters.display,
      DefaultRootWindow(windowParameters.display),
      0, 0,
      800, 600,
      0,
      BlackPixel(windowParameters.display, DefaultScreen(windowParameters.display)),
      WhitePixel(windowParameters.display, DefaultScreen(windowParameters.display))
    );

    if (windowParameters.window == 0) {
      std::cout << "Unable to create window\n";
      return false;
    }

    return true;
  }

  bool DestroyWindow(
    VkInstance const & vulkanInstance,
    WindowParameters & windowParameters,
    VkSurfaceKHR & surface
  ) {
    vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
    XDestroyWindow(windowParameters.display, windowParameters.window);
    XCloseDisplay(windowParameters.display);

    return true;
  }
}
