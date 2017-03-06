cmake_minimum_required(VERSION 2.6)

include_directories( "${PROJECT_SOURCE_DIR}/src" "${Parallel_INCLUDE_DIRS}")
add_executable( test_communicator test_communicator.cpp)
target_link_libraries( test_communicator  Parallel "${EXTRA_LIBS}")

if(EXTRA_COMPILE_FLAGS)
  set_target_properties(test_communicator PROPERTIES
    COMPILE_FLAGS "${EXTRA_COMPILE_FLAGS}")
endif(EXTRA_COMPILE_FLAGS)

if(EXTRA_LINK_FLAGS)
  set_target_properties(test_communicator PROPERTIES
    LINK_FLAGS "${EXTRA_LINK_FLAGS}")
endif(EXTRA_LINK_FLAGS)

