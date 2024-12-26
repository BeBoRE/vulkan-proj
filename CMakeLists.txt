cmake_minimum_required(VERSION 3.31.1)
project(VulkanProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)

add_compile_definitions(VK_NO_PROTOTYPES)

file(GLOB source CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")

add_executable(${PROJECT_NAME} ${source})

target_include_directories(${PROJECT_NAME}
PRIVATE
${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

