cmake_minimum_required(VERSION 3.9)
project(mytest)

set(CMAKE_CXX_STANDARD 11)

#set(OpenCV_FOUND 1)
find_package( OpenCV REQUIRED )
add_executable(mytest main.cpp people.cpp people.h)

#add_executable(OpenCVTest ${SOURCE_FILES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries( mytest ${OpenCV_LIBS} )