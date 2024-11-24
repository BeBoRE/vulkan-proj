#include <iostream>
#include <dlfcn.h>

int main()
{
  #if defined _WIN32
  HMODULE vuklan_library = LoadLibrary("vulkan-1.dll");
  #elif defined __linux
  void* vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
  #endif

  if (vulkan_library == nullptr) {
    std::cout << "Could not connect with a Vulkan Runtime library.\n";
    return false;
  }

  #ifdef NDEBUG
    printf("Release configuration!\n");
    #else
    printf("Debug configuration!\n");
    #endif

  std::cout << "bladibla";

  return true;
}
