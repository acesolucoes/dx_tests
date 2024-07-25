cmake_minimum_required(VERSION 3.20)

# Project name
project(UnityPluginShader)

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

# set xxxxxxxxxxxxxxxxxxxx
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE d3d11.lib d3dcompiler.lib )
target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Unity )

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
###################

message(STATUS "VS WINDOWS INFO: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

# TODO: to see how access the
if(FXC_EXECUTABLE)
    message(STATUS "FXC.exe PATH already in cache: ${FXC_EXECUTABLE}")
else()
    find_program(FXC_EXECUTABLE
                NAMES fxc
    )

    if( NOT FXC_EXECUTABLE)
        message(FATAL_ERROR "FXC.exe not found: ${FXC_EXECUTABLE}")
    endif()
endif()

# Generates header based on compute shader HLSL code
execute_process(
    COMMAND "${FXC_EXECUTABLE}" /Tcs_5_0 /ECSMain SimpleComputeShader.hlsl /Fh OutCompute.h /Qstrip_reflect /Qstrip_debug /Qstrip_priv
    OUTPUT_VARIABLE FXC_EXEC_OUTPUT
    ERROR_VARIABLE FXC_EXEC_ERROR
    RESULT_VARIABLE FXC_EXEC_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)