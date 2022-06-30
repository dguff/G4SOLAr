######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : lunedì giu 27, 2022 13:48:51 CEST
######################################################################

# SLArQReadout dictionary 
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

ROOT_GENERATE_DICTIONARY(SLArQDiffusionDict
  ${PROJECT_SOURCE_DIR}/include/SLArQDiffusion.hh
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/SLArQDiffusionLinkDef.h)

add_library(SLArQDiffusion
  SHARED
  ${PROJECT_SOURCE_DIR}/src/SLArQDiffusion.cc
  SLArQDiffusionDict.cxx
  )
target_link_libraries(SLArQDiffusion ${ROOT_LIBRARIES} SLArQReadout ${SLArMCPrimaryInfo})

ROOT_GENERATE_DICTIONARY(SLArQEvRecoDict
  ${PROJECT_SOURCE_DIR}/include/SLArQEvReco.hh
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/SLArQEvRecoLinkDef.h)

add_library(SLArQEvReco
  SHARED
  ${PROJECT_SOURCE_DIR}/src/SLArQEvReco.cc
  SLArQEvRecoDict.cxx
  )

target_link_libraries(SLArQEvReco ${ROOT_LIBRARIES} ROOT::GenVector SLArQReadout)
