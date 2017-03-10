# Copyright 2017 Dr. Xavier JUVIGNY

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Parallel CXX)

# Version number
SET (Parallel_VERSION_MAJOR 0)
SET (Parallel_VERSION_MINOR 1)

OPTION (USE_MPI "Use MPI for the parallel functions call. That's a stub else." ON)

IF (USE_MPI)
    FIND_PACKAGE(MPI REQUIRED)
    INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
    SET (EXTRA_LIBS ${EXTRA_LIBS} ${MPI_LIBRARIES})
    ADD_DEFINITIONS( -DUSE_MPI=1 )
    IF (MPI_COMPILE_FLAGS)
        SET (EXTRA_COMPILE_FLAGS ${EXTRA_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
    ENDIF (MPI_COMPILE_FLAGS)
    IF (MPI_LINK_FLAGS)
        SET (EXTRA_LINK_FLAGS ${EXTRA_LINK_FLAGS} ${MPI_LINK_FLAGS})
    ENDIF (MPI_LINK_FLAGS)
  ENDIF (USE_MPI)

  # add a target to generate API documentation with Doxygen
  FIND_PACKAGE(Doxygen)
  if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM
      )
  endif(DOXYGEN_FOUND)

SET(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include  CACHE INTERNAL "${PROJECT_NAME}:Include Directories" FORCE)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(tests)
