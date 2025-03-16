cmake_minimum_required(VERSION 3.25)
project(2d-platformer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### Create Executable
add_executable(${PROJECT_NAME}
  src/renderer/renderer.cpp
  src/renderer/buffer.cpp
  src/renderer/texture.cpp
  src/renderer/tools.cpp
  src/renderer/material.cpp
  src/renderer/shader.cpp
  src/renderer/pipeline.cpp
  src/shader_defs/shader_pairs.cpp
  src/rect.cpp
  src/window.cpp
  src/util.cpp
  src/image.cpp
  src/assets.cpp
  src/view.cpp
  src/main.cpp
  # Adding SPIRV-Reflect directly to project
  third-party/SPIRV-Reflect-vulkan-sdk/spirv_reflect.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
  third-party/stb
  third-party/SPIRV-Reflect-vulkan-sdk
  third-party/glm)

### Vulkan setup
find_package(Vulkan REQUIRED)

### VMA setup
add_subdirectory(third-party/VulkanMemoryAllocator)

### vk-boostrap
add_subdirectory(third-party/vk-bootstrap)

### SDL setup
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
add_subdirectory(third-party/SDL EXCLUDE_FROM_ALL)

### Glm
add_subdirectory(third-party/glm)

### Link libs
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
    vk-bootstrap::vk-bootstrap
    SDL3::SDL3
    glm::glm)

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

# Copy necessary data to project source, better way to do this later?
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_SOURCE_DIR}/build)