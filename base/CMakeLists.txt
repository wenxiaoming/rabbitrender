cmake_minimum_required(VERSION 2.8)
project(Base)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

#set(OS_LINUX TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # -std=c++11 instead of -std=gnu++11
set(MY_CXX_COMPILE_FEATURES cxx_generic_lambdas cxx_range_for cxx_strong_enums)

if(OS_WIN)
    MESSAGE("Build for Windows")
    add_definitions(-DOS_WIN)
endif()

if(OS_LINUX)
    MESSAGE("Build for Linux")
    add_definitions(-DOS_LINUX)
endif()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

if(NOT OS_WIN)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall   -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -g -O0")
endif()

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

if(NOT OS_WIN)
# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    add_definitions(-DCOMPILEDWITHC11)
    message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    add_definitions(-DCOMPILEDWITHC0X)
    message(STATUS "Using flag -std=c++0x.")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

ENDIF()

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    #if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    #endif()
endif()

file(GLOB_RECURSE BASE_SRCS 
    ascii_trie.cc 
    ascii_trie.h 
    backtrace.h     
    base32.cc     
    base32.h     
    build_config.h     
    closure.h     
    compiler_specific.h     
    concurrent_message_loop.cc     
    concurrent_message_loop.h     
    delayed_task.cc     
    delayed_task.h     
    eintr_wrapper.h     
    file.cc     
    file.h     
    hash_combine.h     
    #icu_util.cc     
    #icu_util.h     
    log_level.h     
    log_settings.cc     
    log_settings.h     
    log_settings_state.cc     
    logging.cc     
    logging.h     
    make_copyable.h     
    mapping.cc     
    mapping.h     
    memory/ref_counted.h     
    memory/ref_counted_internal.h     
    memory/ref_ptr.h     
    memory/ref_ptr_internal.h     
    memory/task_runner_checker.cc     
    memory/task_runner_checker.h     
    memory/thread_checker.h     
    memory/weak_ptr.h     
    memory/weak_ptr_internal.cc     
    memory/weak_ptr_internal.h     
    message_loop.cc     
    message_loop.h     
    message_loop_impl.cc     
    message_loop_impl.h     
    message_loop_task_queues.cc     
    message_loop_task_queues.h     
    native_library.h     
    paths.cc     
    paths.h     
    posix_wrappers.h     
    raster_thread_merger.cc     
    raster_thread_merger.h     
    size.h     
    synchronization/atomic_object.h     
    synchronization/count_down_latch.cc     
    synchronization/count_down_latch.h     
    synchronization/semaphore.cc     
    synchronization/semaphore.h     
    synchronization/shared_mutex.h     
    synchronization/sync_switch.cc     
    synchronization/sync_switch.h     
    synchronization/waitable_event.cc     
    synchronization/waitable_event.h     
    task_queue_id.h     
    task_runner.cc     
    task_runner.h     
    task_source.cc     
    task_source.h     
    thread.cc     
    thread.h     
    thread_local.cc     
    thread_local.h     
    time/time_delta.h     
    time/time_point.cc     
    time/time_point.h    
    command_line.cc     
    #trace_event.cc     
    #trace_event.h     
    unique_fd.cc     
    unique_fd.h     
    unique_object.h     
    wakeable.h)

if (OS_WIN)
    list(APPEND BASE_SRCS
        backtrace_stub.cc)
else()
    list(APPEND BASE_SRCS
        backtrace.cc)
endif()

if (OS_MAC OR OS_IOS)
    list(APPEND BASE_SRCS
    platform/posix/shared_mutex_posix.cc)
else()
    list(APPEND BASE_SRCS
    synchronization/shared_mutex_std.cc)
endif()

if (OS_MAC OR OS_IOS)
    #cflags_objc = flutter_cflags_objc
    #cflags_objcc = flutter_cflags_objcc
    list(APPEND BASE_SRCS
    platform/darwin/cf_utils.cc
    platform/darwin/cf_utils.h
    platform/darwin/message_loop_darwin.h
    platform/darwin/message_loop_darwin.mm
    platform/darwin/paths_darwin.mm
    platform/darwin/platform_version.h
    platform/darwin/platform_version.mm
    platform/darwin/scoped_block.h
    platform/darwin/scoped_block.mm
    platform/darwin/scoped_nsobject.h
    platform/darwin/scoped_nsobject.mm
    platform/darwin/string_range_sanitization.h
    platform/darwin/string_range_sanitization.mm)
endif()

if (OS_ANDROID)
    list(APPEND BASE_SRCS
    platform/android/jni_util.cc
    platform/android/jni_util.h
    platform/android/jni_weak_ref.cc
    platform/android/jni_weak_ref.h
    platform/android/message_loop_android.cc
    platform/android/message_loop_android.h
    platform/android/paths_android.cc
    platform/android/paths_android.h
    platform/android/scoped_java_ref.cc
    platform/android/scoped_java_ref.h
    platform/linux/timerfd.cc
    platform/linux/timerfd.h)
endif()

if(OS_LINUX)
    list(APPEND BASE_SRCS
    platform/linux/message_loop_linux.cc
    platform/linux/message_loop_linux.h
    platform/linux/paths_linux.cc
    platform/linux/timerfd.cc
    platform/linux/timerfd.h)
endif()

if(OS_LINUX)
    list(APPEND BASE_SRCS
    perf/profiler.h
    perf/profiler.cpp
    perf/compiler.h
    perf/benchmark.h
    perf/performancecounters.h)
endif()



#[[
if (is_fuchsia) {
        sources += [
          "platform/fuchsia/message_loop_fuchsia.cc",
          "platform/fuchsia/message_loop_fuchsia.h",
          "platform/fuchsia/paths_fuchsia.cc",
        ]
    
        public_deps += [
          "$fuchsia_sdk_root/pkg:async-cpp",
          "$fuchsia_sdk_root/pkg:async-loop-cpp",
          "$fuchsia_sdk_root/pkg:async-loop-default",
          "$fuchsia_sdk_root/pkg:syslog",
          "$fuchsia_sdk_root/pkg:trace",
          "$fuchsia_sdk_root/pkg:trace-engine",
          "$fuchsia_sdk_root/pkg:zx",
        ]
}]]

if(OS_WIN)
   add_definitions(-DOS_WIN)
   add_definitions(-DUNICODE -D_UNICODE)
   add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
   list(APPEND BASE_SRCS
        platform/win/errors_win.cc
        platform/win/errors_win.h
        platform/win/file_win.cc
        platform/win/mapping_win.cc  
        platform/win/message_loop_win.cc  
        platform/win/message_loop_win.h  
        platform/win/native_library_win.cc  
        platform/win/paths_win.cc  
        platform/win/posix_wrappers_win.cc  
        platform/win/wstring_conversion.h)
else()

list(APPEND BASE_SRCS
        platform/posix/file_posix.cc
        platform/posix/mapping_posix.cc
        platform/posix/native_library_posix.cc
        platform/posix/paths_posix.cc
        platform/posix/posix_wrappers_posix.cc)
endif()

#message(STATUS "BASE_SRCS: ${BASE_SRCS}")

include_directories(
    ${PROJECT_SOURCE_DIR}
)
#add_library(${PROJECT_NAME} SHARED ${BASE_SRCS})

add_library(${PROJECT_NAME} STATIC ${BASE_SRCS})

if(OS_WIN)
    target_link_libraries(${PROJECT_NAME} Rpcrt4)
else()
    target_link_libraries(${PROJECT_NAME})
endif()

# unit test
file(GLOB_RECURSE TEST_SRCS 
    ascii_trie_unittests.cc
    backtrace_unittests.cc 
    base32_unittest.cc 
    command_line_unittest.cc 
    file_unittest.cc 
    hash_combine_unittests.cc 
    logging_unittests.cc 
    mapping_unittests.cc 
    memory/ref_counted_unittest.cc 
    memory/task_runner_checker_unittest.cc 
    memory/weak_ptr_unittest.cc 
    message_loop_task_queues_merge_unmerge_unittests.cc 
    message_loop_task_queues_unittests.cc 
    message_loop_unittests.cc 
    paths_unittests.cc 
    raster_thread_merger_unittests.cc 
    synchronization/count_down_latch_unittests.cc 
    synchronization/semaphore_unittest.cc 
    synchronization/sync_switch_unittest.cc 
    synchronization/waitable_event_unittest.cc 
    task_source_unittests.cc 
    thread_local_unittests.cc 
    thread_unittests.cc 
    time/time_delta_unittest.cc 
    time/time_point_unittest.cc 
    time/time_unittest.cc)

if (OS_MAC)
    list(APPEND TEST_SRCS
        platform/darwin/cf_utils_unittests.mm 
        platform/darwin/string_range_sanitization_unittests.mm)
endif()

if (OS_LINUX)
    list(APPEND TEST_SRCS
        perf/benchmark.cpp)
endif()

if (NOT OS_WIN)
find_package(GTest REQUIRED)
add_executable(base_test ${TEST_SRCS})
target_link_libraries(base_test ${GTEST_BOTH_LIBRARIES} ${PROJECT_NAME} pthread dl)
endif()

if (OS_MAC)
target_link_libraries(base_test "-framework CoreFoundation")
endif()