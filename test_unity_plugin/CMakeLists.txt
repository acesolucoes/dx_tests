cmake_minimum_required(VERSION 3.20)

# Project name
project(UnityPluginExample)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the source file
set(SOURCES
    RenderAPI.h
    PlatformBase.h
    OutCompute.h

    RenderingPlugin.cpp
    D3D11Render.cpp
)

option(USE_STRUCTURED_BUFFERS "Use structured buffers in tne shader by default " True)
option(TESTING_DOUBLE "Test pass double data to shaders" False)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

find_package(DirectX11 REQUIRED)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE d3d11.lib d3dcompiler.lib )
target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Unity )

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})