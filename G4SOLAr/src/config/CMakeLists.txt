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

#----------------------------------------------------------------------
# Install instructions

set(G4SOLAr_cfg_libs
  SLArPDSystemConfig
  )

set(G4SOLAr_cfg_resources 
  ${CMAKE_CURRENT_BINARY_DIR}/SLArPDSystemConfigDict.cxx 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArPDSystemConfigDict.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArPDSystemConfigDict_rdict.pcm
  )

foreach(lib IN LISTS G4SOLAr_cfg_libs)
  install(TARGETS ${lib}
    EXPORT                      G4SOLArTargets
    LIBRARY DESTINATION        "${G4SOLAR_LIB_DIR}" 
    RUNTIME DESTINATION        "${G4SOLAR_LIB_DIR}")
endforeach()

install(FILES ${G4SOLAr_cfg_resources} DESTINATION "${G4SOLAR_LIB_DIR}")
