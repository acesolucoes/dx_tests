cmake_minimum_required(VERSION 3.20)

# Project name
project(DirectComputeExample)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the source file
set(SOURCE_FILE main.cpp)

option(USE_STRUCTURED_BUFFERS "Use structured buffers in tne shader by default " True)
option(TESTING_DOUBLE "Test pass double data to shaders" False)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

find_package(DirectX11 REQUIRED)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILE})

target_link_libraries(${PROJECT_NAME} PRIVATE d3d11.lib d3dcompiler.lib )

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/ExampleCompute.hlsl"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

if( ${USE_STRUCTURED_BUFFERS} )
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_STRUCTURED_BUFFERS)
    MESSAGE(INFORMATION "USING STRUCTURED BUFFERS")
else()
    MESSAGE(INFORMATION "USING RAW BUFFERS")
endif()

if( ${TESTING_DOUBLE} )
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_STRUCTURED_BUFFERS)
    MESSAGE(INFORMATION "USING DOUBLE")
else()
    MESSAGE(INFORMATION "NOT USING DOUBLE")
endif()