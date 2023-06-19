#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(G4SOLAr)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS}
  CMAKE_LIBRARY_OUTPUT_DIRECTORY)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

#----------------------------------------------------------------------------
# Find ROOT package#
find_package(ROOT REQUIRED COMPONENTS RIO Core)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Setup Projects PATHS
set(G4SOLAR_PROJECT_DIR "${PROJECT_SOURCE_DIR}/../../G4SOLAr/")
set(G4SOLAR_INCLUDE_DIR "${G4SOLAR_PROJECT_DIR}/include")
set(G4SOLAR_SRC_DIR     "${G4SOLAR_PROJECT_DIR}/src"    )
if (NOT G4SOLAR_EXT_DIR)
  set(G4SOLAR_EXT_DIR   "${G4SOLAR_PROJECT_DIR}/extern" )
endif()
set(G4SOLAR_LIB_DIR     "${CMAKE_INSTALL_PREFIX}/lib"  )
set(G4SOLAR_BIN_DIR     "${CMAKE_INSTALL_PREFIX}"      )
set(G4SOLAR_ANALSYS_DIR "${G4SOLAR_PROJECT_DIR}/../SOLArAnalysis")
set(G4SOLAR_INSTALL_CMAKE_DIR "${G4SOLAR_LIB_DIR}/cmake/${PROJECT_NAME}")


include_directories(${G4SOLAR_INCLUDE_DIR}
                    ${G4SOLAR_INCLUDE_DIR}/config
                    ${G4SOLAR_INCLUDE_DIR}/event
                    ${Geant4_INCLUDE_DIR}
)

if (CLHEP_EXTERNAL) 
  include_directories(${CLHEP_EXTERNAL})
endif()

#file(GLOB sources   ${G4SOLAR_SRC_DIR}/*.cc
                    #${G4SOLAR_SRC_DIR}/event/*.cc
                    #${G4SOLAR_SRC_DIR}/config/*.cc
                    #${G4SOLAR_SRC_DIR}/material/*.cc
                    #${G4SOLAR_SRC_DIR}/physics/*.cc
                    #${G4SOLAR_SRC_DIR}/detector/*.cc
                    #${G4SOLAR_SRC_DIR}/detector/TPC/*.cc
                    #${G4SOLAR_SRC_DIR}/detector/SuperCell/*.cc
                    #${G4SOLAR_SRC_DIR}/detector/Anode/*.cc
#)

#file(GLOB headers   ${G4SOLAR_INCLUDE_DIR}/*.hh
                    #${G4SOLAR_INCLUDE_DIR}/event/*.hh
                    #${G4SOLAR_INCLUDE_DIR}/physics/*.hh
                    #${G4SOLAR_INCLUDE_DIR}/config/*.hh
                    #${G4SOLAR_INCLUDE_DIR}/material/*.hh
                    #${G4SOLAR_INCLUDE_DIR}/detector/*.hh
                    #${G4SOLAR_INCLUDE_DIR}/detector/TPC/*.hh
                    #${G4SOLAR_INCLUDE_DIR}/detector/SuperCell/*.hh
                    #${G4SOLAR_INCLUDE_DIR}/detector/Anode/*.hh
                    #)

add_definitions(${ROOT_CXX_FLAGS})

#----------------------------------------------------------------------------
# Configure local paths
configure_file(${G4SOLAR_PROJECT_DIR}/SLArUserPath.hh.in ${G4SOLAR_INCLUDE_DIR}/SLArUserPath.hh @ONLY)

#----------------------------------------------------------------------------
# Configure rootlogon
configure_file(${G4SOLAR_INCLUDE_DIR}/rootlogon.C.in ${G4SOLAR_ANALSYS_DIR}/rootlogon.C @ONLY)

#----------------------------------------------------------------------------
# Generate SLArEvent and SLArConfig libraries and dictionaries
add_subdirectory(${G4SOLAR_SRC_DIR}/config ${CMAKE_CURRENT_BINARY_DIR}/config)
add_subdirectory(${G4SOLAR_SRC_DIR}/event  ${CMAKE_CURRENT_BINARY_DIR}/event)

#----------------------------------------------------------------------------
# configure package config
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${G4SOLAR_PROJECT_DIR}/cmake/G4SOLArConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/G4SOLArConfig.cmake
  INSTALL_DESTINATION "${G4SOLAR_INSTALL_CMAKE_DIR}"
  PATH_VARS G4SOLAR_LIB_DIR G4SOLAR_BIN_DIR G4SOLAR_INCLUDE_DIR G4SOLAR_INSTALL_CMAKE_DIR
  )

# extract git branch and commit hash
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/G4SOLArConfig.cmake
  DESTINATION ${G4SOLAR_INSTALL_CMAKE_DIR})

# configure package config
install(EXPORT G4SOLArTargets
  #FILE   ${CMAKE_CURRENT_BINARY_DIR}/cmake/G4SOLArConfig.cmake
  DESTINATION "${G4SOLAR_INSTALL_CMAKE_DIR}"
  NAMESPACE G4SOLAr::
)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

#add_executable(solar_sim solar_sim.cc ${sources} ${headers})
#target_link_libraries(solar_sim ${Geant4_LIBRARIES})
#target_link_libraries(solar_sim ${ROOT_LIBRARIES})
#target_link_libraries(solar_sim SLArMCEvent)
#target_link_libraries(solar_sim SLArScintillation)
#target_link_libraries(solar_sim SLArReadoutSystemConfig)
#target_link_libraries(solar_sim BxDecay0::BxDecay0 BxDecay0::BxDecay0_Geant4)
#target_link_libraries(solar_sim ${MARLEY} ${MARLEY_ROOT})
#target_compile_definitions(solar_sim 
  #PUBLIC 
  #$<$<CONFIG:Debug>:SLAR_DEBUG>
  #$<$<STREQUAL:${Geant4_gdml_FOUND},ON>:SLAR_GDML>
  #$<$<STREQUAL:${SLAR_EXTERNAL},ON>:SLAR_EXTERNAL>
  #SLAR_EXTERNAL_PARTICLE="${SLAR_EXTERNAL_PARTICLE}"
  #PRIVATE
  #"-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\""
  #)

#list(APPEND G4SOLAR_RPATH
  #"${G4SOLAR_LIB_DIR}"
  #"${BXDECAY0_DIR}/lib"
  #"${BXDECAY0_DIR}/lib64"
  #"${MARLEY_DIR}/build")
#set_target_properties(solar_sim PROPERTIES
  #INSTALL_RPATH "${G4SOLAR_RPATH}"
  #BUILD_WITH_INSTALL_RPATH 1
  #)
#install(TARGETS solar_sim
  #LIBRARY DESTINATION ${G4SOLAR_BIN_DIR}
  #RUNTIME DESTINATION ${G4SOLAR_BIN_DIR}
  #)
##----------------------------------------------------------------------------
## Copy all assets (geometry, materials, cfg) to the install directory.
## This is so that we can run the executable directly because it
## relies on these scripts being in the current working directory.

#install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${G4SOLAR_BIN_DIR})


