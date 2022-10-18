########################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists.txt
# @created     : Tuesday Oct 18, 2022 11:31:06 CEST
# @description : Analysis libraries to implement a visibility map 
#                based on the semi-analytical model of light propagation
########################################################################

cmake_minimum_required(VERSION 3.2)
project(SLArAnalysis)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(ROOT REQUIRED COMPONENTS RIO Core MathMore)
include(${ROOT_USE_FILE})

find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

find_package(G4SOLAr REQUIRED)
if(G4SOLAr_FOUND)
  message(STATUS "G4SOLAR found: include at ${G4SOLAR_INCLUDE_DIR}")
  list(APPEND CMAKE_PREFIX_PATH ${G4SOLAR_LIB_DIR})
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${G4SOLAR_INCLUDE_DIR}
    ${Geant4_INCLUDE_DIR}
    ${ROOT_INCLUDE_DIRS}
    )

add_subdirectory(VisMap)

add_executable(test_output ${PROJECT_SOURCE_DIR}/test_output.cc)
target_link_libraries(test_output PRIVATE ${ROOT_LIBRARIES})
target_link_libraries(test_output PRIVATE G4SOLAr::SLArReadoutSystemConfig G4SOLAr::SLArMCEvent)




