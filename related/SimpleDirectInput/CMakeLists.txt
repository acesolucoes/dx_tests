cmake_minimum_required(VERSION 3.10)
project(DirectInputDetectControllers)

set(CMAKE_CXX_STANDARD 17)

add_executable(DirectInputDetectControllers main.cpp)

# Link against DirectInput and DXGUID (needed for GUIDs)
target_link_libraries(DirectInputDetectControllers PRIVATE dinput8 dxguid)
