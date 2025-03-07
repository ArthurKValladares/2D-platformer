cmake_minimum_required(VERSION 3.25)
project(2d-platformer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### Create Executable
set(VERTEX_SHADERS
  shaders/triangle.vert.hlsl)
set(PIXEL_SHADERS
  shaders/triangle.frag.hlsl)

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${VERTEX_SHADERS}
    ${PIXEL_SHADERS}
)
foreach(VERTEX_SHADER ${VERTEX_SHADERS})
  set_property(SOURCE ${VERTEX_SHADER} PROPERTY VS_SHADER_TYPE "Vertex")
endforeach(VERTEX_SHADER)

foreach(PIXEL_SHADER ${PIXEL_SHADERS})
  set_property(SOURCE ${PIXEL_SHADER} PROPERTY VS_SHADER_TYPE "Pixel")
endforeach(PIXEL_SHADER)

### Vulkan setup
find_package(Vulkan REQUIRED)

### VMA setup
add_subdirectory(third-party/VulkanMemoryAllocator)

### Volk setup
if(APPLE)
  set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX)
  if(USE_WAYLAND)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WAYLAND_KHR)
  else()
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XLIB_KHR)
  endif()
elseif(WIN32)
  set(VOLK_STATIC_DEFINES "VK_USE_PLATFORM_WIN32_KHR")
endif()
add_subdirectory(third-party/volk)

### SDL setup
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
add_subdirectory(third-party/SDL EXCLUDE_FROM_ALL)

### Link libs
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
    volk
    SDL3::SDL3)