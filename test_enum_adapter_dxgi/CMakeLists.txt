cmake_minimum_required(VERSION 3.20)

# Project name
project(EnumAdapterDXGI)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the source file
set(SOURCE_FILE main.cpp)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

find_package(DirectX11)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILE})
target_compile_definitions(${PROJECT_NAME} PRIVATE "UNICODE" "_UNICODE")
target_link_libraries(${PROJECT_NAME} PRIVATE "d3d11.lib" "dxgi.lib" "d3dcompiler.lib")

