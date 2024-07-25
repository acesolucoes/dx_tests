cmake_minimum_required(VERSION 3.10)

# Project name and version
project(SDL_DX VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SDL2IMAGE_AVIF OFF)

message(STATUS "SOURCE DIR: ENV{SOURCE_DIR}")
add_subdirectory(SDL)
add_subdirectory(SDL_image)

macro(ADD_SDL_SAMPLE target sources)
    add_executable(${target} ${sources})

    target_compile_definitions(${target} PRIVATE UNICODE _UNICODE)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/SDL/include)

    # Link necessary libraries
    target_link_libraries(${target} PRIVATE d3d11 d3dcompiler dxgi dxguid)
    target_link_libraries(${target} PRIVATE SDL2 SDL2main SDL2_image)

    # Set the output directory
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    # Copy shaders to the output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2main> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2_image> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )    
endmacro(ADD_SDL_SAMPLE target)

add_subdirectory(simple_dx_sdl_test)
add_subdirectory(simple_dx_sdl2)

if(MSVC)
    add_compile_options(CL=/MP)
endif()

