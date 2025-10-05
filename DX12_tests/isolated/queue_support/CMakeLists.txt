cmake_minimum_required(VERSION 3.16)
project(Dx12QueueSupport)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(add_dx12_executable NAME)
    # Add the executable
    add_executable(${NAME} ${ARGN})

    # Link Direct3D 12 and DXGI libraries
    target_link_libraries(${NAME} PRIVATE d3d12 dxgi dxguid)

    # Include Windows headers
    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

    # Set output directory for the executable
    set_target_properties(${NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    # Add additional options for Windows builds
    if (WIN32)
        target_compile_definitions(${NAME} PRIVATE WIN32_LEAN_AND_MEAN)
    endif()
endmacro()

add_dx12_executable(Dx12QueueSupport main.cpp)

