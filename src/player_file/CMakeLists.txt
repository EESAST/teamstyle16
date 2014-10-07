cmake_minimum_required (VERSION 2.6)
project (teamstyle16_ai)

# Directory of the boost library shipped
set(SHIPPED_BOOST_ROOT ${PROJECT_SOURCE_DIR}/boost_1_56_0)

# Try to use shipped Boost library if the generator is MSVC, which indicates
# the user might not be familiar with Boost
if(MSVC)
	if(MSVC12)
		if(CMAKE_CL_64)
			set(BOOST_LIBRARY_SUBDIR lib64-msvc-12.0)
		else()
			set(BOOST_LIBRARY_SUBDIR lib32-msvc-12.0)
		endif()
	elseif(MSVC11)
		set(BOOST_LIBRARY_SUBDIR lib64-msvc-11.0)
	elseif(MSVC10)
		set(BOOST_LIBRARY_SUBDIR lib64-msvc-10.0)
	elseif(MSVC90)
		set(BOOST_LIBRARY_SUBDIR lib64-msvc-9.0)
	elseif(MSVC80)
		set(BOOST_LIBRARY_SUBDIR lib64-msvc-8.0)
	endif ()

	if (BOOST_LIBRARY_SUBDIR)
		message("Supported MSVC version detected, trying to use shipped Boost library")
		set(BOOST_ROOT ${SHIPPED_BOOST_ROOT})
		set(BOOST_LIBRARYDIR ${SHIPPED_BOOST_ROOT}/${BOOST_LIBRARY_SUBDIR})
	endif()

	# To avoid an error
	add_definitions(-D_WIN32_WINNT=0x0501)
endif()

message("Boost configurations:")
message("    BOOST_ROOT: " ${BOOST_ROOT})
message("    BOOST_LIBRARYDIR: " ${BOOST_LIBRARYDIR})

find_package(Boost 1.56.0 REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-DBOOST_ALL_NO_LIB)

add_library(ai_basic basic.cpp connection.cpp)
add_library(main main.cpp)

add_executable (debugger debugger.cpp)
add_executable (empty_ai ai.cpp)
target_link_libraries (debugger ai_basic main ${Boost_LIBRARIES})
target_link_libraries (empty_ai ai_basic main ${Boost_LIBRARIES})
