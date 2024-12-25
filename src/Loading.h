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
  bool ConnectVulkanLoader(LIBRARY_TYPE & vulkan_library);
  bool LoadExportedVulkanLoaderLibaryFunction(LIBRARY_TYPE const & vulkan_library);
  bool LoadGlobalLevelFunctions();
  bool LoadAvailableInstanceExtensions( std::vector<VkExtensionProperties> & available_extensions );
  bool CheckDesiredExtensions( std::vector<VkExtensionProperties> & available_extensions, std::vector<char const *> & desired_extensions );
}
