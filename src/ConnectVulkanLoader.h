#if defined _WIN32
    typedef HMODULE LIBRARY_TYPE;
#elif defined __linux
    typedef void* LIBRARY_TYPE;
#endif

namespace VulkanProject {
  bool ConnectVulkanLoader(LIBRARY_TYPE vulkan_library);
}
