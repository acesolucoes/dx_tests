cmake_minimum_required(VERSION 3.20)

# Project name
project(HelloDX12)

# link_libraries(d3d12.lib dxgi.lib d3dcompiler.lib)
# Set C++ standard
# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/directx")

# Define the source file
add_subdirectory(hello_dx12)