cmake_minimum_required(VERSION 3.31.1)
project(VulkanProject VERSION 0.1 LANGUAGES CXX)

find_package(ZLIB REQUIRED)

add_library(library STATIC library/src/A.cpp)
target_include_directories(library PUBLIC library/include)
target_link_libraries(library 
  PRIVATE
    ZLIB::ZLIB
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    library
)
