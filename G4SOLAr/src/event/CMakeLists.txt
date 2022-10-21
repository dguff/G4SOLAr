######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : venerdì mag 06, 2022 23:01:03 CEST
######################################################################

# SLArMCPrimaryInfo dictionary 
ROOT_GENERATE_DICTIONARY(SLArMCPrimaryInfoDict
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCPrimaryInfo.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventTrajectory.hh
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCPrimaryInfoLinkDef.h)

add_library(SLArMCPrimaryInfo
  SHARED
  ${PROJECT_SOURCE_DIR}/src/event/SLArMCPrimaryInfo.cc
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventTrajectory.cc
  SLArMCPrimaryInfoDict.cxx
  )
target_link_libraries(SLArMCPrimaryInfo ${ROOT_LIBRARIES})

#ROOT_GENERATE_DICTIONARY(SLArPhotonHitsDict
  #LINKDEF 
  #${PROJECT_SOURCE_DIR}/include/event/SLArEventPhotonHitLinkDef.h)
#add_library(SLArEventPhotonHit 
  #SHARED ${PROJECT_SOURCE_DIR}/src/event/SLArEventPhotonHit.cc)
#target_link_libraries(SLArEventPhotonHit PUBLIC ROOT::Core)

ROOT_GENERATE_DICTIONARY(SLArMCEventReadoutDict
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventPhotonHit.hh

  ${PROJECT_SOURCE_DIR}/include/event/SLArEventTile.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventMegatile.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventReadoutTileSystem.hh
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventReadoutLinkDef.h
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventSuperCell.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventSuperCellSystem.hh
  )
add_library(SLArMCEventReadout
  SHARED
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventPhotonHit.cc
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventTile.cc
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventMegatile.cc
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventReadoutTileSystem.cc
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventSuperCell.cc
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventSuperCellSystem.cc
  SLArMCEventReadoutDict.cxx
  )
target_link_libraries(
  SLArMCEventReadout 
  PUBLIC 
  SLArReadoutSystemConfig ${ROOT_LIBRARIES})

# Master MC EVENT
ROOT_GENERATE_DICTIONARY(SLArMCEventDict
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCEvent.hh
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCEventLinkDef.h
  )
add_library(SLArMCEvent 
  SHARED
  ${PROJECT_SOURCE_DIR}/src/event/SLArMCEvent.cc
  SLArMCEventDict.cxx
  )
target_link_libraries(SLArMCEvent 
                      ${ROOT_LIBRARIES} 
                      SLArMCPrimaryInfo
                      SLArMCEventReadout)

#----------------------------------------------------------------------------
# Install instructions

set(G4SOLAr_event_libs
  SLArMCEvent
  SLArMCPrimaryInfo
  SLArMCEventReadout
  )

set(G4SOLAr_event_resources 
  ${CMAKE_CURRENT_BINARY_DIR}/SLArMCPrimaryInfoDict.cxx 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCPrimaryInfoDict.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCPrimaryInfoDict_rdict.pcm
  ${CMAKE_CURRENT_BINARY_DIR}/SLArMCEventDict.cxx 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCEventDict.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCEventDict_rdict.pcm
  ${CMAKE_CURRENT_BINARY_DIR}/SLArMCEventReadoutDict.cxx 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCEventReadoutDict.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCEventReadoutDict_rdict.pcm
  )

foreach(lib IN LISTS G4SOLAr_event_libs)
  install(TARGETS ${lib}
    EXPORT                      G4SOLArTargets
    LIBRARY DESTINATION        "${G4SOLAR_LIB_DIR}" 
    RUNTIME DESTINATION        "${G4SOLAR_LIB_DIR}")
endforeach()

install(FILES ${G4SOLAr_event_resources} DESTINATION "${G4SOLAR_LIB_DIR}")
