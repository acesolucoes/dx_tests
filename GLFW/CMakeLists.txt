#cmake -B build -S . -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=e:\src\dep_manager.cmake
cmake_minimum_required(VERSION 3.16)
project(GLFW_Direct3D12)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable
add_executable(GLFW_Direct3D12 main.cpp)

# Find the GLFW package
find_package(glfw3 REQUIRED)

# Link GLFW
target_link_libraries(GLFW_Direct3D12 PRIVATE glfw)

# Include the necessary directories
target_include_directories(GLFW_Direct3D12 PRIVATE ${GLFW_INCLUDE_DIRS})

# Link Direct3D 12 and DXGI libraries
target_link_libraries(GLFW_Direct3D12 PRIVATE d3d12 dxgi dxguid)

# Ensure proper symbols on Windows
if (WIN32)
    target_compile_definitions(GLFW_Direct3D12 PRIVATE GLFW_EXPOSE_NATIVE_WIN32)
endif()

# Output directory
set_target_properties(GLFW_Direct3D12 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
