cmake_minimum_required(VERSION 3.31.1)
project(VulkanProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)

add_compile_definitions(VK_NO_PROTOTYPES)

if( NOT DEFINED VK_USE_PLATFORM )
	if( WIN32 )
		set( VK_USE_PLATFORM "WIN32" )
	elseif( LINUX )
		set( VK_USE_PLATFORM "XLIB" )
	endif()
endif()

message( STATUS "VK_USE_PLATFORM: ${VK_USE_PLATFORM}" )

if( VK_USE_PLATFORM STREQUAL "VK_USE_PLATFORM_WIN32_KHR" )
	set( CMAKE_C_FLAGS_RELWITHDEBINFO    "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT" )
	set( CMAKE_C_FLAGS_RELEASE           "${CMAKE_C_FLAGS_RELEASE} /MT" )
	set( CMAKE_C_FLAGS_DEBUG             "${CMAKE_C_FLAGS_DEBUG} /MTd" )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT" )
	set( CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} /MT" )
	set( CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} /MTd" )
	set( PLATFORM_LIBRARY                "" )
elseif( VK_USE_PLATFORM STREQUAL "XLIB" )
	set( PLATFORM_LIBRARY "X11" )
endif()

add_definitions( -DVK_USE_PLATFORM_${VK_USE_PLATFORM}_KHR )

if( CMAKE_BUILD_TYPE STREQUAL "debug" )
	add_definitions(-D_DEBUG)
endif()

file(GLOB source CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")

add_executable(${PROJECT_NAME} ${source})

target_include_directories(${PROJECT_NAME}
PRIVATE
${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${PLATFORM_LIBRARY})

