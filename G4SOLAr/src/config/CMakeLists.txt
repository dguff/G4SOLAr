######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : mercoledì giu 22, 2022 15:14:17 CEST
######################################################################

# SLArReadoutSystemConfig dictionary 
add_library(SLArReadoutSystemConfig
  SHARED
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgBaseModule.cc
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgBaseModule.hh
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgSuperCell.cc
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgSuperCell.hh
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgReadoutTile.cc
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgReadoutTile.hh
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgAssembly.cc
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgAssembly.hh 
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgSuperCellArray.cc
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgSuperCellArray.hh
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgMegaTile.cc
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgMegaTile.hh
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgBaseSystem.cc
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgBaseSystem.hh
  ${PROJECT_SOURCE_DIR}/src/config/SLArCfgAnode.cc
  ${PROJECT_SOURCE_DIR}/include/config/SLArCfgAnode.hh
  )

target_link_libraries(SLArReadoutSystemConfig PUBLIC ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__SLArReadoutSystemConfig 
  ${PROJECT_SOURCE_DIR}/include/config/*.hh
  MODULE SLArReadoutSystemConfig
  LINKDEF ${PROJECT_SOURCE_DIR}/include/config/SLArReadoutSystemConfigLinkDef.h)


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
