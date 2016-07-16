cmake_minimum_required(VERSION 2.8)

project(CarDetection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

IF(UNIX)
  # add some standard warnings
  ADD_DEFINITIONS(-Wno-variadic-macros -Wno-long-long -Wall -Wextra -Winit-self -Woverloaded-virtual -Wsign-promo -Wno-unused-parameter -pedantic -Woverloaded-virtual -Wno-unknown-pragmas)

  # if you like to have warinings about conversions, e.g. double->int or double->float etc., or float compare
  #ADD_DEFINITIONS(-Wconversion -Wfloat-equal)
endif(UNIX)

find_package(OpenCV REQUIRED)
#find_package(CURL REQUIRED)

if(${OpenCV_VERSION} VERSION_LESS 2.3.1)
  message (FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
endif()

file(GLOB main main.cpp)

file(GLOB_RECURSE analysis package_analysis/*.cpp)

file(GLOB_RECURSE bgs package_bgs/*.cpp package_bgs/*.c)
file(GLOB_RECURSE bgs_include package_bgs/*.h)

file(GLOB_RECURSE tracking package_tracking/*.cpp package_tracking/*.c)
file(GLOB_RECURSE tracking_include package_tracking/*.h)

include_directories(${CMAKE_SOURCE_DIR})

add_library(CarDetectionLib SHARED ${tracking} ${bgs} ${analysis} package_analysis/Lane.cpp package_analysis/Lane.h)
target_link_libraries(CarDetectionLib ${OpenCV_LIBS})
set_property(TARGET CarDetectionLib PROPERTY PUBLIC_HEADER ${bgs_include} ${tracking_include})

add_executable(CarDetection ${main})
target_link_libraries(CarDetection ${OpenCV_LIBS} CarDetectionLib)
set_target_properties(CarDetection PROPERTIES OUTPUT_NAME CarDetection)
