cmake_minimum_required(VERSION 2.6) 
project(OpenCVjp_Samples)

cmake_policy(SET CMP0002 OLD)

set(REQ_OPENCV_VER 2.0.0)

include(${CMAKE_ROOT}/Modules/FindPkgConfig.cmake OPTIONAL)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(OPENCV opencv)
  if(NOT OPENCV_FOUND OR OPENCV_VERSION VERSION_LESS REQ_OPENCV_VER)
    message(FATAL_ERROR "Could not find OpenCV>=" ${REQ_OPENCV_VER})
  endif()
endif()
add_subdirectory(c)
add_subdirectory(cpp)



