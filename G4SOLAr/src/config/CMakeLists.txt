######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : Wed Jun 22, 2022 15:14:17 CEST
######################################################################

# SLArReadoutSystemConfig dictionary 
add_library(SLArReadoutSystemConfig
  SHARED
  ${G4SOLAR_SRC_DIR}/config/SLArCfgBaseModule.cc
  ${G4SOLAR_INCLUDE_DIR}/config/SLArCfgBaseModule.hh
  ${G4SOLAR_SRC_DIR}/config/SLArCfgSuperCell.cc
  ${G4SOLAR_INCLUDE_DIR}/config/SLArCfgSuperCell.hh
  ${G4SOLAR_SRC_DIR}/config/SLArCfgReadoutTile.cc
  ${G4SOLAR_INCLUDE_DIR}/config/SLArCfgReadoutTile.hh
  ${G4SOLAR_SRC_DIR}/config/SLArCfgAssembly.cc
  ${G4SOLAR_INCLUDE_DIR}/config/SLArCfgAssembly.hh 
  ${G4SOLAR_SRC_DIR}/config/SLArCfgSuperCellArray.cc
  ${G4SOLAR_INCLUDE_DIR}/config/SLArCfgSuperCellArray.hh
  ${G4SOLAR_SRC_DIR}/config/SLArCfgMegaTile.cc
  ${G4SOLAR_INCLUDE_DIR}/config/SLArCfgMegaTile.hh
  ${G4SOLAR_SRC_DIR}/config/SLArCfgBaseSystem.cc
  ${G4SOLAR_INCLUDE_DIR}/config/SLArCfgBaseSystem.hh
  ${G4SOLAR_SRC_DIR}/config/SLArCfgAnode.cc
  ${G4SOLAR_INCLUDE_DIR}/config/SLArCfgAnode.hh
  )

target_link_libraries(SLArReadoutSystemConfig PUBLIC ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__SLArReadoutSystemConfig 
  ${G4SOLAR_INCLUDE_DIR}/config/*.hh
  MODULE SLArReadoutSystemConfig
  LINKDEF ${G4SOLAR_INCLUDE_DIR}/config/SLArReadoutSystemConfigLinkDef.h)


#----------------------------------------------------------------------
# Install instructions

set(G4SOLAr_cfg_libs
  SLArReadoutSystemConfig
  )

set(G4SOLAr_cfg_resources 
  ${CMAKE_CURRENT_BINARY_DIR}/G__SLArReadoutSystemConfig.cxx
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArReadoutSystemConfig.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArReadoutSystemConfig_rdict.pcm
  )

foreach(lib IN LISTS G4SOLAr_cfg_libs)
  install(TARGETS ${lib}
    EXPORT                      G4SOLArTargets
    LIBRARY DESTINATION        "${G4SOLAR_LIB_DIR}" 
    RUNTIME DESTINATION        "${G4SOLAR_LIB_DIR}")
endforeach()

install(FILES ${G4SOLAr_cfg_resources} DESTINATION "${G4SOLAR_LIB_DIR}")
