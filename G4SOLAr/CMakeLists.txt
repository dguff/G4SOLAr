#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(G4SOLAr)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS} 
  CMAKE_LIBRARY_OUTPUT_DIRECTORY)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(ROOT REQUIRED COMPONENTS RIO Core)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})
#----------------------------------------------------------------------------
# Find external dependencies
include(ExternalProject)
ExternalProject_Add("rapidjson"
  SOURCE_DIR      ${PROJECT_SOURCE_DIR}/extern/rapidjson
  BINARY_DIR      ${CMAKE_CURRENT_BINARY_DIR}/extern/rapidjson
  GIT_REPOSITORY  "https://github.com/Tencent/rapidjson.git"
  BUILD_COMMAND   cmake -E echo "rapidjson: Skipping build step."
  INSTALL_COMMAND cmake -E echo "rapidjson: Skipping install step."
  )
set(G4SOLAR_RAPIDJSON_INCLUDE "${PROJECT_SOURCE_DIR}/extern/rapidjson/include")

find_package(BxDecay0 REQUIRED COMPONENTS Geant4 CONFIG) 

set (G4SOLAR_MARLEY $ENV{MARLEY})
message(STATUS "libMARLEY path: ${G4SOLAR_MARLEY}/build")
find_library(MARLEY 
             NAMES MARLEY
             PATHS "${G4SOLAR_MARLEY}/build"
             REQUIRED
             NO_DEFAULT_PATH) 
           find_library(MARLEY_ROOT 
             NAMES MARLEY_ROOT
             PATHS "${G4SOLAR_MARLEY}/build"
             REQUIRED
             NO_DEFAULT_PATH) 

if (MARLEY) 
  message(STATUS "MARLEY library found")
endif()

set(G4SOLAR_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(G4SOLAR_SRC_DIR     "${PROJECT_SOURCE_DIR}/src"    )
set(G4SOLAR_LIB_DIR     "${CMAKE_INSTALL_PREFIX}/lib"  )
set(G4SOLAR_BIN_DIR     "${CMAKE_INSTALL_PREFIX}"      )
set(G4SOLAR_INSTALL_CMAKE_DIR "${G4SOLAR_LIB_DIR}/cmake/${PROJECT_NAME}")

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${G4SOLAR_INCLUDE_DIR}
                    ${G4SOLAR_INCLUDE_DIR}/material
                    ${G4SOLAR_INCLUDE_DIR}/detector
                    ${G4SOLAR_INCLUDE_DIR}/detector/Tank
                    ${G4SOLAR_INCLUDE_DIR}/detector/SuperCell
                    ${G4SOLAR_INCLUDE_DIR}/config
                    ${G4SOLAR_INCLUDE_DIR}/event
                    ${BxDecay0_INCLUDE_DIR}
                    ${G4SOLAR_MARLEY}/include
                    ${G4SOLAR_RAPIDJSON_INCLUDE}
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS}
)

file(GLOB sources   ${G4SOLAR_SRC_DIR}/*.cc
                    ${G4SOLAR_SRC_DIR}/event/*.cc
                    ${G4SOLAR_SRC_DIR}/config/*.cc
                    ${G4SOLAR_SRC_DIR}/material/*.cc
                    ${G4SOLAR_SRC_DIR}/detector/*.cc
                    ${G4SOLAR_SRC_DIR}/detector/Tank/*.cc
                    ${G4SOLAR_SRC_DIR}/detector/SuperCell/*.cc
)

file(GLOB headers   ${G4SOLAR_INCLUDE_DIR}/*.hh
                    ${G4SOLAR_INCLUDE_DIR}/event/*.hh
                    ${G4SOLAR_INCLUDE_DIR}/config/*.hh
                    ${G4SOLAR_INCLUDE_DIR}/material/*.hh
                    ${G4SOLAR_INCLUDE_DIR}/detector/*.hh
                    ${G4SOLAR_INCLUDE_DIR}/detector/Tank/*.hh
                    ${G4SOLAR_INCLUDE_DIR}/detector/SuperCell/*.hh
                    )

add_definitions(${ROOT_CXX_FLAGS})

#----------------------------------------------------------------------------
# Configure local paths
configure_file(SLArUserPath.hh.in ${G4SOLAR_INCLUDE_DIR}/SLArUserPath.hh @ONLY)

#----------------------------------------------------------------------------
# Generate SLArEvent and SLArConfig libraries and dictionaries
add_subdirectory(${G4SOLAR_SRC_DIR}/config)
add_subdirectory(${G4SOLAR_SRC_DIR}/event)

#----------------------------------------------------------------------------
# configure package config
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/G4SOLArConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/G4SOLArConfig.cmake
  INSTALL_DESTINATION "${G4SOLAR_INSTALL_CMAKE_DIR}"
  PATH_VARS G4SOLAR_LIB_DIR G4SOLAR_BIN_DIR G4SOLAR_INCLUDE_DIR G4SOLAR_INSTALL_CMAKE_DIR
  )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/G4SOLArConfig.cmake 
  DESTINATION ${G4SOLAR_INSTALL_CMAKE_DIR})

# configure package config
export(EXPORT G4SOLArTargets
  FILE "${G4SOLAR_INSTALL_CMAKE_DIR}/G4SOLArTargets.cmake"
  NAMESPACE G4SOLAr::
)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(solar_sim solar_sim.cc ${sources} ${headers})
add_dependencies(solar_sim rapidjson)
target_link_libraries(solar_sim ${Geant4_LIBRARIES})
target_link_libraries(solar_sim ${ROOT_LIBRARIES})
target_link_libraries(solar_sim SLArMCEvent)
target_link_libraries(solar_sim SLArPDSystemConfig)
target_link_libraries(solar_sim BxDecay0::BxDecay0 BxDecay0::BxDecay0_Geant4)
target_link_libraries(solar_sim ${MARLEY} ${MARLEY_ROOT})
set_target_properties(solar_sim PROPERTIES 
  INSTALL_RPATH ${G4SOLAR_LIB_DIR}
  BUILD_WITH_INSTALL_RPATH 1
  )
install(TARGETS solar_sim 
  LIBRARY DESTINATION ${G4SOLAR_BIN_DIR}
  RUNTIME DESTINATION ${G4SOLAR_BIN_DIR}
  )
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build beamCell. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#

file(COPY ${PROJECT_SOURCE_DIR}/macros DESTINATION ${G4SOLAR_BIN_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/macros/vis.mac DESTINATION ${G4SOLAR_BIN_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/marley_cfg DESTINATION ${G4SOLAR_BIN_DIR})

