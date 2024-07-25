cmake_minimum_required(VERSION 3.10)

# Project name and version
project(DirectX11Cube VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Add executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)

# Link necessary libraries
target_link_libraries(${PROJECT_NAME} d3d11)
target_link_libraries(${PROJECT_NAME} d3dcompiler)
target_link_libraries(${PROJECT_NAME} dxgi)
target_link_libraries(${PROJECT_NAME} dxguid)

# Set the output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy shaders to the output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)