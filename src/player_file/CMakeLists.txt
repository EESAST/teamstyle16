cmake_minimum_required (VERSION 2.6)
project (teamstyle16_ai)

find_path (BOOST_INCLUDE_DIR boost/asio.hpp
    "${PROJECT_SOURCE_DIR}/include}"
)

find_library (BOOST_SYSTEM_LIB boost_system
    "${PROJECT_SOURCE_DIR}/lib"
)

include_directories ("${BOOST_INCLUDE_DIR}")

add_library(ai_basic basic.cpp connection.cpp)
add_library(main main.cpp)

add_executable (debugger debugger.cpp)
add_executable (empty_ai ai.cpp)
target_link_libraries (debugger ai_basic main ${BOOST_SYSTEM_LIB})
target_link_libraries (empty_ai ai_basic main ${BOOST_SYSTEM_LIB})
