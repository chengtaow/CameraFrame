cmake_minimum_required (VERSION 3.8)

add_executable (CameraFrame "CameraFrame.cpp" "CameraFrame.hpp" "Test.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CameraFrame PROPERTY CXX_STANDARD 20)
endif()
