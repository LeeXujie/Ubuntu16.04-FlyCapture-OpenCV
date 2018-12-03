cmake_minimum_required(VERSION 2.8)

project(flycapture_test)

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )

FIND_PACKAGE(OpenCV REQUIRED)

MESSAGE("OpenCV_DIR = " ${OpenCV_DIR})
MESSAGE("OpenCV_INCLUDE_DIRS = " ${OpenCV_INCLUDE_DIRS})
MESSAGE("OpenCV_LIBS = " ${OpenCV_LIBS})

INCLUDE_DIRECTORIES(
    include
    ${OpenCV_INCLUDE_DIRS}
    /usr/include/flycapture
)

add_executable(${PROJECT_NAME} src/camnode.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS} /usr/lib/libflycapture.so )
