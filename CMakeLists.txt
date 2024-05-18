cmake_minimum_required(VERSION 3.2)
project(rabbitrender C CXX)

add_subdirectory("compiler/frontend")

# include directories
include_directories(${CMAKE_INCLUDE_PATH})
include_directories("include")
include_directories("include/gles")

# initial variables
add_definitions(-DGL_GLEXT_PROTOTYPES)
add_definitions(-DEGL_EGLEXT_PROTOTYPES)
add_definitions(-DHAVE_PTHREADS)

if(NOT MSVC)
add_definitions(-DHAVE_POSIX_CLOCKS)
endif()
# Generic compilation options
if(MSVC)
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
  add_definitions(-DNOMINMAX)
  #add_definitions(-DWIN64)
  add_definitions(-DSWRENDER_EXPORTS)
  add_definitions(-DHAVE_MS_C_RUNTIME)
  add_definitions(-DHAVE_PTHREADS)
  add_definitions(-D_USE_MATH_DEFINES)
  add_definitions(-DEGL_EGLEXT_PROTOTYPES)
  add_definitions(-DGL_GLEXT_PROTOTYPES)
  add_definitions(-DHAVE_STRUCT_TIMESPEC)
  add_definitions(-DHAVE_LRINT)

  # regeneration does not work well with msbuild custom rules.
  set(CMAKE_SUPPRESS_REGENERATION ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
  if(USE_MSVC_MT)
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
  endif()
  # Disable common MSVC warnings
  # Integer conversion warnings(e.g. int64 to int)
  add_compile_options(/wd4244)
  add_compile_options(/wd4267)
  # Signed unsigned constant comparison
  add_compile_options(/wd4018)
  # Aligned alloc may not met(need c++17)
  add_compile_options(/wd4316)
  # unreferenced local variables(usually in exception catch)
  add_compile_options(/wd4101)
  # always inline keyword not necessary
  add_compile_options(/wd4180)
  # DLL interface warning in c++
  add_compile_options(/wd4251)
  include_directories("pthreads/include")
else(MSVC)
  set(WARNING_FLAG -Wall)
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Build in Debug mode")
    set(CMAKE_C_FLAGS "-O0 -g ${WARNING_FLAG} -fPIC ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-O0 -g ${WARNING_FLAG} -fPIC ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CUDA_FLAGS "-O0 -g -Xcompiler=-Wall -Xcompiler=-fPIC ${CMAKE_CUDA_FLAGS}")
  else()
    set(CMAKE_C_FLAGS "-O2 ${WARNING_FLAG} -fPIC ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-O2 ${WARNING_FLAG} -fPIC ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CUDA_FLAGS "-O2 -Xcompiler=-Wall -Xcompiler=-fPIC ${CMAKE_CUDA_FLAGS}")
    set(TVM_VISIBILITY_FLAG "")
    if (HIDE_PRIVATE_SYMBOLS)
      message(STATUS "Hide private symbols...")
      set(TVM_VISIBILITY_FLAG "-fvisibility=hidden")
    endif(HIDE_PRIVATE_SYMBOLS)
  endif ()
  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND
      CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
    set(CMAKE_CXX_FLAGS "-faligned-new ${CMAKE_CXX_FLAGS}")
  endif()

endif(MSVC)

# add source group
FILE(GLOB_RECURSE GROUP_SOURCE "src/*.cc")
FILE(GLOB_RECURSE GROUP_INCLUDE "src/*.h" "include/*.h")
#assign_source_group("Source" ${GROUP_SOURCE})
#assign_source_group("Include" ${GROUP_INCLUDE})

# Source file lists
file(GLOB_RECURSE COMPILER_SRCS
    libpixelflinger/tinyutils/*.cpp
    libpixelflinger/*.cpp
    )

file(GLOB_RECURSE WINDOWS_SRCS 
    libpixelflinger/gettimeofday.cpp)

if(NOT MSVC)
list(REMOVE_ITEM COMPILER_SRCS ${WINDOWS_SRCS})
endif()

file(GLOB SWRNDER_SRCS
    swrender/*.cpp
    )

if(MSVC)
file(GLOB WINRENDER_SRCS
    windisplay/EGLUtils.cpp
    windisplay/Framebuffer.cpp
    windisplay/EsUtilWin.cpp
    windisplay/FramebufferWin.cpp
    )
else()
file(GLOB WINRENDER_SRCS
    windisplay/EGLUtils.cpp
    windisplay/FramebufferSDL.cpp
    windisplay/EsUtilSDL.cpp
    windisplay/Framebuffer.cpp
    )
endif()


list(APPEND COMPILER_SRCS ${SWRNDER_SRCS})

list(APPEND COMPILER_SRCS ${WINRENDER_SRCS})

include(CheckCXXCompilerFlag)
if(NOT MSVC)
  check_cxx_compiler_flag("-std=c++14" SUPPORT_CXX14)
  set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CUDA_STANDARD 14)
else()
  check_cxx_compiler_flag("/std:c++14" SUPPORT_CXX14)
  set(CMAKE_CXX_FLAGS "/std:c++14 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CUDA_STANDARD 14)
endif()

if(NOT MSVC)
set(CMAKE_CXX_FLAGS "-g -v ${CMAKE_CXX_FLAGS}")
endif()

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

message(STATUS "COMPILER_SRCS: ${COMPILER_SRCS}")

#add_library(rabbitrender_objs OBJECT ${COMPILER_SRCS})
if(NOT MSVC)
add_library(rabbitrender SHARED ${COMPILER_SRCS})
else()
add_library(rabbitrender STATIC ${COMPILER_SRCS})
endif()
#target_compile_definitions(rabbitrender_objs PUBLIC "NDEBUG")
if(NOT MSVC)
target_link_libraries(rabbitrender PUBLIC gcc)
endif()

if(MSVC)
link_directories(${CMAKE_SOURCE_DIR}/pthreads/lib)
target_link_libraries(rabbitrender PUBLIC pthreadVC2)
endif()

add_subdirectory("gl_basic")
add_subdirectory("particle")
add_subdirectory("rotatecube")

add_subdirectory("third_party/llvm-10.0")