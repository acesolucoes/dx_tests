cmake_minimum_required(VERSION 3.10)
project(XInputDetectControllers)

set(CMAKE_CXX_STANDARD 17)

add_executable(XInputDetectControllers main.cpp)

# Link against XInput (Windows system library)
target_link_libraries(XInputDetectControllers PRIVATE Xinput9_1_0)
