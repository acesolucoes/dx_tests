cmake_minimum_required(VERSION 3.20)

# Project name
project(DirectComputeExample)

# Set C++ standard
# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_CXX_EXTENSIONS OFF)

# Define the source file
set(SOURCE_FILE dx12_example.c)

# find_package(DirectX11 REQUIRED)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILE})

target_compile_definitions(${PROJECT_NAME} PUBLIC _USE_MATH_DEFINES)

# target_include_directories(${PROJECT_NAME} PUBLIC "E:/Windows Kits/10/Include/10.0.22621.0/um")
# target_link_directories(${PROJECT_NAME} PUBLIC "E:/Windows Kits/10/Lib/10.0.22621.0/um/x64")

target_link_libraries(${PROJECT_NAME} PRIVATE d3d12.lib dxgi.lib d3dcompiler.lib )

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/ExampleCompute.hlsl"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)