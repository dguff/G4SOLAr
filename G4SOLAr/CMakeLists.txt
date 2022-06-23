#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(G4SOLAr)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS} 
  CMAKE_LIBRARY_OUTPUT_DIRECTORY)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
#include(${CMAKE_CURRENT_BINARY_DIR}/extern/rapidjson/RapidJSONConfig.cmake)

find_package(BxDecay0 REQUIRED COMPONENTS Geant4 CONFIG) 

set(G4SOLAR_INCLUDE "${PROJECT_SOURCE_DIR}/include")
set(G4SOLAR_SRC     "${PROJECT_SOURCE_DIR}/src"    )

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${G4SOLAR_INCLUDE}
                    ${G4SOLAR_INCLUDE}/material
                    ${G4SOLAR_INCLUDE}/detector
                    ${G4SOLAR_INCLUDE}/detector/Tank
                    ${G4SOLAR_INCLUDE}/detector/SuperCell
                    ${G4SOLAR_INCLUDE}/config
                    ${G4SOLAR_INCLUDE}/event
                    ${BxDecay0_INCLUDE_DIR}
                    ${G4SOLAR_RAPIDJSON_INCLUDE}
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS}
)

file(GLOB sources   ${G4SOLAR_SRC}/*.cc
                    ${G4SOLAR_SRC}/event/*.cc
                    ${G4SOLAR_SRC}/config/*.cc
                    ${G4SOLAR_SRC}/material/*.cc
                    ${G4SOLAR_SRC}/detector/*.cc
                    ${G4SOLAR_SRC}/detector/Tank/*.cc
                    ${G4SOLAR_SRC}/detector/SuperCell/*.cc
)

file(GLOB headers   ${G4SOLAR_INCLUDE}/*.hh
                    ${G4SOLAR_INCLUDE}/event/*.hh
                    ${G4SOLAR_INCLUDE}/config/*.hh
                    ${G4SOLAR_INCLUDE}/material/*.hh
                    ${G4SOLAR_INCLUDE}/detector/*.hh
                    ${G4SOLAR_INCLUDE}/detector/Tank/*.hh
                    ${G4SOLAR_INCLUDE}/detector/SuperCell/*.hh
                    )

add_definitions(${ROOT_CXX_FLAGS})

#----------------------------------------------------------------------------
# Configure local paths
configure_file(SLArUserPath.hh.in ${G4SOLAR_INCLUDE}/SLArUserPath.hh @ONLY)

#----------------------------------------------------------------------------
# Generate SLArEvent and SLArConfig libraries and dictionaries
add_subdirectory(${G4SOLAR_SRC}/config)
add_subdirectory(${G4SOLAR_SRC}/event)

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

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build beamCell. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(g4solar_SCRIPTS
    optPhoton.mac
    gui.mac
    vis.mac
  )

foreach(_script ${g4solar_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
install(TARGETS solar_sim DESTINATION bin)

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


