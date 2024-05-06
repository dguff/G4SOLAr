#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(SOLArUtils)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS}
  CMAKE_LIBRARY_OUTPUT_DIRECTORY)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
if (CMAKE_BUILD_TYPE STREQUAL "Debug") 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  message(STATUS "DEBUG: Adding '-g' option for gdb debugging")
endif()

if (SLAR_PROFILE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
  message(STATUS "Building solar_sim with gprof profiling feature")
endif()

find_package(ROOT REQUIRED COMPONENTS RIO Core)
include(${ROOT_USE_FILE})

find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

find_package(G4SOLAr CONFIG REQUIRED)
if(G4SOLAr_FOUND)
  message(STATUS "G4SOLAR found: include at ${G4SOLAR_INCLUDE_DIR}")
  list(APPEND CMAKE_PREFIX_PATH ${G4SOLAR_BIN_DIR})
endif()

find_package(RapidJSON REQUIRED)

#----------------------------------------------------------------------------
# Define path variables

list(APPEND CMAKE_LIBRARY_PATH ${G4SOLAR_LIB_DIR})
set(SOLAR_UTILS_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(SOLAR_UTILS_BIN_DIR "${CMAKE_INSTALL_PREFIX}")
set(SOLAR_UTILS_INC_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOLAR_UTILS_SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(SOLAR_UTILS_SCRIPT_DIR "${PROJECT_SOURCE_DIR}/script")
set(SOLAR_UTILS_INSTALL_CMAKE_DIR "${SOLAR_UTILS_LIB_DIR}/cmake/${PROJECT_NAME}")

include(ExternalProject)
ExternalProject_Add("progressbar"
  SOURCE_DIR      ${CMAKE_CURRENT_BINARY_DIR}/progressbar
  INSTALL_DIR     ${SOLAR_UTILS_INC_DIR}
  UPDATE_DISCONNECTED 1
  GIT_REPOSITORY  "https://github.com/dguff/progressbar.git"
  GIT_TAG         "main"
  CONFIGURE_COMMAND echo "configuring progressbar"
  BUILD_COMMAND echo "fetching progressbar"
  INSTALL_COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/progressbar/include/progressbar.hpp ${SOLAR_UTILS_INC_DIR} )

include_directories(
  ${SOLAR_UTILS_INC_DIR}
  ${G4SOLAR_INCLUDE_DIR}
  ${Geant4_INCLUDE_DIR}
  ${ROOT_INCLUDE_DIRS}
  "${RapidJSON_INCLUDE_DIR}"
  )
if (CLHEP_EXTERNAL) 
  include_directories(${CLHEP_EXTERNAL})
endif()

link_directories(${ROOT_LIBRARY_DIR})
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# configure rootlogon and setup script
configure_file(
  ${PROJECT_SOURCE_DIR}/rootlogon.C.in 
  ${PROJECT_SOURCE_DIR}/rootlogon.C @ONLY)
#----------------------------------------------------------------------------
# Build executables
add_subdirectory(${SOLAR_UTILS_SRC_DIR})
add_subdirectory(${SOLAR_UTILS_SCRIPT_DIR})

