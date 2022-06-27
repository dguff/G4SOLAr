######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : lunedì giu 27, 2022 13:48:51 CEST
######################################################################

# SLArMCPrimaryInfo dictionary 
ROOT_GENERATE_DICTIONARY(SLArQReadoutDict
  ${PROJECT_SOURCE_DIR}/include/SLArQReadout.hh
  ${PROJECT_SOURCE_DIR}/include/SLArQCluster.hh
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/SLArQReadoutLinkDef.h)

add_library(SLArQReadout
  SHARED
  ${PROJECT_SOURCE_DIR}/src/SLArQReadout.cc
  ${PROJECT_SOURCE_DIR}/src/SLArQCluster.cc
  SLArQReadoutDict.cxx
  )
target_link_libraries(SLArQReadout ${ROOT_LIBRARIES})


