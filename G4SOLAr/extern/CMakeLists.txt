######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : mercoledì lug 06, 2022 15:51:36 CEST
######################################################################

cmake_minimum_required(VERSION 3.4)
project(G4SOLAr_external) 

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS} 
  CMAKE_LIBRARY_OUTPUT_DIRECTORY)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Geant4 REQUIRED)
find_package(ROOT REQUIRED COMPONENTS RIO Core)

if (NOT CMAKE_INSTALL_PREFIX) 
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
endif()

message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

include(ExternalProject)
ExternalProject_Add("rapidjson"
  SOURCE_DIR      ${PROJECT_SOURCE_DIR}/src/rapidjson
  BINARY_DIR      ${CMAKE_CURRENT_BINARY_DIR}/rapidjson-build
  INSTALL_DIR     ${CMAKE_INSTALL_PREFIX}/rapidjson
  UPDATE_DISCONNECTED 0
  GIT_REPOSITORY  "https://github.com/Tencent/rapidjson.git"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/rapidjson
  BUILD_COMMAND cmake --build . -j 4
  )

ExternalProject_Add("bxdecay0"
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/bxdecay0
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bxdecay0-build
  INSTALL_DIR     ${PROJECT_SOURCE_DIR}/bxdecay0
  GIT_REPOSITORY "https://github.com/BxCppDev/bxdecay0.git"
  GIT_TAG "develop"
  UPDATE_DISCONNECTED 0
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/bxdecay0 -DBXDECAY0_WITH_GEANT4_EXTENSION="ON" -DGeant4_DIR="${Geant4_DIR}"
  BUILD_COMMAND cmake --build . -j 4
  )

ExternalProject_Add("marley"
  SOURCE_DIR ${CMAKE_INSTALL_PREFIX}/marley
  BINARY_DIR ${CMAKE_INSTALL_PREFIX}/marley/build
  GIT_REPOSITORY "https://github.com/MARLEY-MC/marley.git"
  GIT_TAG "main"
  UPDATE_DISCONNECTED 0
  CONFIGURE_COMMAND cmake -E echo "MARLEY: Skipping configure command"
  BUILD_COMMAND make -j4
  INSTALL_COMMAND cmake -E echo "MARLEY: Skipping install step"
  )


