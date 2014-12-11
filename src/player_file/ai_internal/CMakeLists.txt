cmake_minimum_required(VERSION 2.6)
project(ai_internal)

option(with-c++11 "Build using standard C++11" OFF)

# Try to use shipped Boost library by default, making it easy for everyone
# to compile an AI
option(with-shipped-boost "Build using shipped version of Boost library" ON)

# Whether to build tests with gtests
# Note that this implicitly indicates 'with-c++11' option, because of the
# impliment of connection_test requires <random> & <thread>, which might be
# an unnecessary burden if use Boost.random/thread instead
option(with-tests "Build with tests for ai" OFF)
if(with-tests)
    set(with-c++11 ON)
endif(with-tests)

# Configure options
if(with-c++11)
    # On MSVC no compile options need to be given
    if(MSVC)
        if (MSVC_VERSION LESS 1800 AND with-tests)
            message(WARNING "Currently only VC++ 12.0 or higher are supported "
                            "to build tests\n"
                            "Tests will not be built")
            set(with-tests OFF)
        endif()
    else(MSVC)
        # As far as I know, Clang, GNU & Intel use this option to support C++11
        add_compile_options(-std=c++11)
    endif(MSVC)
endif(with-c++11)

# Configure Boost library
# Directory of the boost library shipped
set(SHIPPED_BOOST_ROOT ${PROJECT_SOURCE_DIR}/boost_1_56_0)

if(MSVC)
    # To avoid an warning in VS
    add_definitions(-D_WIN32_WINNT=0x0501)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)

    if(with-shipped-boost)
        # Try shipped Boost library
        if(MSVC12)
            if(CMAKE_CL_64)
                set(BOOST_LIBRARY_SUBDIR lib64-msvc-12.0)
            else ()
                set(BOOST_LIBRARY_SUBDIR lib32-msvc-12.0)
            endif()
        elseif(MSVC11)
            set(BOOST_LIBRARY_SUBDIR lib32-msvc-11.0)
        elseif(MSVC10)
            set(BOOST_LIBRARY_SUBDIR lib32-msvc-10.0)
        elseif(MSVC90)
            set(BOOST_LIBRARY_SUBDIR lib32-msvc-9.0)
        elseif(MSVC80)
            set(BOOST_LIBRARY_SUBDIR lib32-msvc-8.0)
        endif()
    endif(with-shipped-boost)

    if(BOOST_LIBRARY_SUBDIR)
        message("Supported MSVC version detected, trying to use shipped Boost library")
        set(BOOST_ROOT ${SHIPPED_BOOST_ROOT})
        set(BOOST_LIBRARYDIR ${SHIPPED_BOOST_ROOT}/${BOOST_LIBRARY_SUBDIR})
    endif()
endif(MSVC)

message("Boost configurations:")
message("    BOOST_ROOT: " ${BOOST_ROOT})
message("    BOOST_LIBRARYDIR: " ${BOOST_LIBRARYDIR})

add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.55.0 REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})
# Done with Boost library

# On linux Boost.asio need to be linked with pthread, so this is a (hopefully)
# portable solution
find_package(Threads)

# Configure protobuf
set(PROTOBUF_ROOT ${PROJECT_SOURCE_DIR}/protobuf-2.5.0 CACHE PATH "Location of protobuf sources.")
include_directories(${PROJECT_SOURCE_DIR}/protobuf-2.5.0/src)
add_subdirectory(protobuf)

# Build libraries
add_library(ai_base basic.cpp connection.cpp communicate.pb.cc)
target_link_libraries(ai_base ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} libprotobuf)

add_library(ai_driver main.cpp)
target_link_libraries(ai_driver ai_base)

# Build tests with gtest if required
if(with-tests)
    set(gtest_force_shared_crt ON CACHE BOOL "force gtest to use shared CRT")
    add_subdirectory(gtest-1.7.0)
    add_executable(ai_test ai.cpp basic_test.cpp connection_test.cpp)
    target_include_directories(ai_test PUBLIC ${PROJECT_SOURCE_DIR}/gtest-1.7.0/include)
    target_link_libraries(ai_test ai_base gtest gtest_main)
endif(with-tests)

# Build empty_ai, sample_ai & debugger
add_executable (empty_ai empty_ai.cpp)
add_executable (sample_ai sample_ai.cpp)
add_executable (debugger debugger.cpp)
target_link_libraries (empty_ai ai_driver)
target_link_libraries (sample_ai ai_driver)
target_link_libraries (debugger ai_driver)
