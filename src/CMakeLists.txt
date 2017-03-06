cmake_minimum_required(VERSION 2.6)

include_directories( "${PROJECT_SOURCE_DIR}/include")
add_library( Parallel SHARED "Context.cpp" "Communicator.cpp")

SET_PROPERTY(TARGET Parallel PROPERTY CXX_STANDARD 14)


install (TARGETS Parallel DESTINATION lib)
