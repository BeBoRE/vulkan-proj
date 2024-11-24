cmake_minimum_required(VERSION 3.31.1)
project(VulkanProject VERSION 0.1 LANGUAGES CXX)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(VulkanProject ${CMAKE_DL_LIBS})
