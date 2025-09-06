cmake_minimum_required(VERSION 3.20)

# Project name
project(AmbientLighting)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set (DX_APP_SOURCES
		inputclass.cpp
		inputclass.h
		systemclass.cpp
		systemclass.h
		applicationclass.cpp
		applicationclass.h
		d3dclass.cpp
		d3dclass.h
		lightshaderclass.cpp
		lightshaderclass.h
		cameraclass.cpp
		cameraclass.h
		modelclass.cpp
		modelclass.h
		textureclass.cpp
		textureclass.h
		lightclass.cpp
		lightclass.h
		main.cpp
)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

find_package(DirectX11)

# Add the executable
add_executable(${PROJECT_NAME} WIN32 ${DX_APP_SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE "UNICODE" "_UNICODE")
target_link_libraries(${PROJECT_NAME} PRIVATE "d3d11.lib" "dxgi.lib" "d3dcompiler.lib")


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Engine/data" 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/light.ps" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Engine/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/light.vs" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Engine/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/stone03.tga" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Engine/data/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/cube.txt" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Engine/data/"
)
