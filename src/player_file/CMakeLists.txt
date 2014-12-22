cmake_minimum_required(VERSION 2.6)
project(teamstyle16_ai)

include_directories(ai)  # For basic.h

add_subdirectory(ai_internal)  # Build internal libs/executables
add_subdirectory(ai)  # Build ai
