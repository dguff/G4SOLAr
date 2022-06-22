######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : mercoledì giu 22, 2022 15:14:17 CEST
######################################################################

# SLArPDSystemConfig dictionary 
ROOT_GENERATE_DICTIONARY(SLArPDSystemConfigDict
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgSuperCell.hh
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgSuperCellArray.hh
  ${PROJECT_SOURCE_DIR}/include/config/SLArPDSystemConfig.hh
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/config/SLArPDSystemConfigLinkDef.h)

add_library(SLArPDSystemConfig
  SHARED
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgSuperCell.cc
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgSuperCellArray.cc
  ${PROJECT_SOURCE_DIR}/src/config/SLArPDSystemConfig.cc
  SLArPDSystemConfigDict.cxx
  )
target_link_libraries(SLArPDSystemConfig ${ROOT_LIBRARIES})


