######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : lunedì giu 27, 2022 13:22:48 CEST
######################################################################

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(G4SOLArAnalysis)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS} 
  CMAKE_LIBRARY_OUTPUT_DIRECTORY)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ROOT REQUIRED COMPONENTS RIO Core)

#----------------------------------------------------------------------------
# Setup ROOT include directories and compile definitions
include(${ROOT_USE_FILE})
#----------------------------------------------------------------------------
# Find external dependencies
find_library(SLArMCEvent 
  NAMES SLArMCEvent PATHS ${PROJECT_SOURCE_DIR}/../build/src/event REQUIRED)
find_library(SLArMCPrimaryInfo 
  NAMES SLArMCPrimaryInfo PATHS ${PROJECT_SOURCE_DIR}/../build/src/event REQUIRED)
#----------------------------------------------------------------------------
# Set local variable names
set(G4SOLAR_INCLUDE          "${PROJECT_SOURCE_DIR}/../G4SOLAr/include")
set(G4SOLAR_ANALYSIS_INCLUDE "${PROJECT_SOURCE_DIR}/include")
set(G4SOLAR_ANALYSIS_SRC     "${PROJECT_SOURCE_DIR}/src"    )
set(G4SOLAR_ANALYSIS_MACROS  "${PROJECT_SOURCE_DIR}/macros" ) 

#----------------------------------------------------------------------------
# Locate sources and headers for this project
include_directories( 
  ${G4SOLAR_ANALYSIS_INCLUDE}
  ${G4SOLAR_INCLUDE}
  )


add_definitions(${ROOT_CXX_FLAGS})

#----------------------------------------------------------------------------
# Configure local paths
configure_file(${G4SOLAR_ANALYSIS_INCLUDE}/rootlogon.C.in ${PROJECT_SOURCE_DIR}/macros/rootlogon.C @ONLY)

#----------------------------------------------------------------------------
# Generate SLArEvent and SLArConfig libraries and dictionaries
add_subdirectory(${G4SOLAR_ANALYSIS_SRC})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build beamCell. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
#----------------------------------------------------------------------------
# Copy shared libraries and dictionaries to offline/lib folder
#
#add_custom_command(
  #TARGET SLArMCEvent 
  #POST_BUILD
  #COMMAND ${CMAKE_COMMAND} -E copy 
  #$<TARGET_FILE:SLArMCEvent> libSLArMCEventDict_rdict.pcm libSLArMCEventDict.rootmap
  #${PROJECT_SOURCE_DIR}/../offline/lib/
  #)


