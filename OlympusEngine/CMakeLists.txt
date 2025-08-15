cmake_minimum_required(VERSION 3.16)
project(MonsOlympusEngine)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(add_dx12_executable NAME)
    # Add the executable
    add_executable(${NAME} WIN32 ${ARGN})

    # Link Direct3D 12 and DXGI libraries
    target_link_libraries(${NAME} PRIVATE d3d12 dxgi dxguid)

    target_compile_definitions(${NAME} PRIVATE _UNICODE)

    target_precompile_headers(${NAME} PRIVATE src/pch.h)

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

macro(add_dx12_library NAME)
    # Add the executable
    add_library(${NAME} SHARED ${ARGN})

    # Link Direct3D 12 and DXGI libraries
    target_link_libraries(${NAME} PRIVATE d3d12 dxgi dxguid)

    target_compile_definitions(${NAME} PRIVATE _UNICODE)

    target_precompile_headers(${NAME} PRIVATE src/olympus.h)

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

add_dx12_executable(Episode1 src/WinMain.cpp)
add_dx12_executable(Episode2 src/WinMainEpisode2.cpp)
add_dx12_executable(Episode3 
    src/application.cpp 
    src/application.h  
    src/WinMainEpisode3.cpp
    src/pch.cpp
    src/pch.h
    resources/myresource.rc
)
add_dx12_library(Olympus 
    src/olympus.h
    src/olympus.cpp
    src/platform/WIN32/IApplication.h 
    src/platform/WIN32/IApplication.cpp 
    src/platform/WIN32/WinEntry.h
    src/common/logger.cpp 
    src/common/logger.h
    src/common/time.cpp 
    src/common/time.h
    src/core/perGameSettings.h 
    src/core/perGameSettings.cpp 
    src/pch.cpp
    src/pch.h
)
target_include_directories(Olympus PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")
target_compile_definitions(Olympus PRIVATE BUILD_DLL)
target_link_libraries(Episode3 PRIVATE Olympus)

add_custom_command(
    TARGET Episode3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/utilities/MTail.exe"
    "$<TARGET_FILE_DIR:Episode3>"
)

