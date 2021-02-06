cmake_minimum_required(VERSION 2.6)

project(rotatecube CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-g -v ${CMAKE_CXX_FLAGS}")

set(PROJ_EXEC_NAME rotatecube)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_BUILD_TYPE Debug)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++11 ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif ()

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/../include/*"
                          "${PROJECT_SOURCE_DIR}/../pthreads/include"
                          "${PROJECT_SOURCE_DIR}/../frameworks/base/include"
                          "${PROJECT_SOURCE_DIR}/../hardware/libhardware/include"
                          "${PROJECT_SOURCE_DIR}/../frameworks/base/native/include"
                          "${PROJECT_SOURCE_DIR}/../system/core/include"
                          "${PROJECT_SOURCE_DIR}/windisplay")

include_directories("${PROJECT_SOURCE_DIR}/../include")
include_directories("${PROJECT_SOURCE_DIR}/../pthreads/include")
include_directories("${PROJECT_SOURCE_DIR}/../include/frameworks/base/include")

include_directories("${PROJECT_SOURCE_DIR}/../include/hardware/libhardware/include")
include_directories("${PROJECT_SOURCE_DIR}/../include/frameworks/base/native/include")

include_directories("/home/kevin/all/mygithub/rabbitrender/include/frameworks/base/include")

include_directories("${PROJECT_SOURCE_DIR}/../include/system/core/include")

include_directories("${PROJECT_SOURCE_DIR}/../windisplay")

file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/*.cpp")

message(STATUS "SRCS: ${SRCS}")
message(STATUS "HEADERS: ${HEADERS}")

link_directories("/home/kevin/all/mygithub/build")

add_executable(${PROJ_EXEC_NAME} ${SRCS})

target_link_libraries(${PROJ_EXEC_NAME} rabbitrender SDL2 pthread dl m c z)