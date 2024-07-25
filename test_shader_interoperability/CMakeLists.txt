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

if( USE_STRUCTURED_BUFFERS OR TESTING_DOUBLE )

    # Use find_program to locate DXC.exe
    if(FXC_EXECUTABLE)
        message(STATUS "FXC.exe PATH already in cache: ${FXC_EXECUTABLE}")
    else()
        find_program(FXC_EXECUTABLE
                    NAMES fxc
        )
    endif()

    # Check if DXC.exe was found
    if(FXC_EXECUTABLE)
        message(STATUS "FXC.exe found: ${FXC_EXECUTABLE}")
    else()
        message(FATAL_ERROR "FXC.exe not found: ${FXC_EXECUTABLE}")
    endif()

    if( USE_STRUCTURED_BUFFERS AND TESTING_DOUBLE )
        execute_process(
            COMMAND "${FXC_EXECUTABLE}" /Tcs_5_0 /ECSMain /D USE_STRUCTURED_BUFFERS=1 /D TEST_DOUBLE=1 ExampleCompute.hlsl /Fh OutCompute.h /Qstrip_reflect /Qstrip_debug /Qstrip_priv
            OUTPUT_VARIABLE FXC_EXEC_OUTPUT
            ERROR_VARIABLE FXC_EXEC_ERROR
            RESULT_VARIABLE FXC_EXEC_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    elseif( USE_STRUCTURED_BUFFERS )
        execute_process(
            COMMAND "${FXC_EXECUTABLE}" /Tcs_5_0 /ECSMain /D USE_STRUCTURED_BUFFERS=1 ExampleCompute.hlsl /Fh OutCompute.h /Qstrip_reflect /Qstrip_debug /Qstrip_priv
            OUTPUT_VARIABLE FXC_EXEC_OUTPUT
            ERROR_VARIABLE FXC_EXEC_ERROR
            RESULT_VARIABLE FXC_EXEC_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    elseif( TESTING_DOUBLE )
        execute_process(
            COMMAND "${FXC_EXECUTABLE}" /Tcs_5_0 /ECSMain ExampleCompute.hlsl /D TEST_DOUBLE=1 /Fh OutCompute.h /Qstrip_reflect /Qstrip_debug /Qstrip_priv
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE FXC_EXEC_OUTPUT
            ERROR_VARIABLE FXC_EXEC_ERROR
            INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ExampleCompute.hlsl"
            RESULT_VARIABLE FXC_EXEC_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ECHO_OUTPUT_VARIABLE
            ECHO_ERROR_VARIABLE
        )
        MESSAGE("OUT: ${FXC_EXEC_OUTPUT} ERROR: ${FXC_EXEC_ERROR} RESULT: ${FXC_EXEC_RESULT}") 
    endif()
endif()



if( ${USE_STRUCTURED_BUFFERS} )
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_STRUCTURED_BUFFERS)
    MESSAGE(INFORMATION "USING STRUCTURED BUFFERS")
else()
    MESSAGE(INFORMATION "USING RAW BUFFERS")
endif()

if( ${TESTING_DOUBLE} )
    target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_DOUBLE)
    MESSAGE(INFORMATION "USING DOUBLE")
else()
    MESSAGE(INFORMATION "NOT USING DOUBLE")
endif()