cmake_minimum_required(VERSION 3.25)
project(2d-platformer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### Create Executable
add_executable(${PROJECT_NAME}
  src/vk_renderer.cpp
  src/window.cpp
  src/util.cpp
  src/main.cpp
)

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

# Compile shaders
find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
    )

message("\n------- Compiling Shaders Step Start -------")
make_directory(${PROJECT_SOURCE_DIR}/build/shaders)
message(STATUS "GLSL Validador Path:\n" ${GLSL_VALIDATOR})
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/build/shaders/${FILE_NAME}.spv")

  message(STATUS "Compiling Shader:\n" ${GLSL})
  message(STATUS "Compile Command:\n" ${GLSL_VALIDATOR} " " -V " " ${GLSL} " " -o " " ${SPIRV})

  execute_process(COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV})
endforeach(GLSL)
message("------- Compiling Shaders Step End -------\n")