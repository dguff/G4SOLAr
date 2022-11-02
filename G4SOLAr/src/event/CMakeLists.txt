######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : venerdì mag 06, 2022 23:01:03 CEST
######################################################################

# SLArMCPrimaryInfo dictionary 
add_library(SLArMCPrimaryInfo
  SHARED
  ${PROJECT_SOURCE_DIR}/src/event/SLArMCPrimaryInfo.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCPrimaryInfo.hh
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventTrajectory.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventTrajectory.hh
  )

target_link_libraries(SLArMCPrimaryInfo PUBLIC ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__SLArMCPrimaryInfo
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCPrimaryInfo.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventTrajectory.hh
  MODULE SLArMCPrimaryInfo
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCPrimaryInfoLinkDef.h)


#ROOT_GENERATE_DICTIONARY(SLArPhotonHitsDict
  #LINKDEF 
  #${PROJECT_SOURCE_DIR}/include/event/SLArEventPhotonHitLinkDef.h)
#add_library(SLArEventPhotonHit 
  #SHARED ${PROJECT_SOURCE_DIR}/src/event/SLArEventPhotonHit.cc)
#target_link_libraries(SLArEventPhotonHit PUBLIC ROOT::Core)

add_library(SLArMCEventReadout
  SHARED
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventPhotonHit.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventPhotonHit.hh
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventTile.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventTile.hh
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventMegatile.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventMegatile.hh
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventReadoutTileSystem.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventReadoutTileSystem.hh
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventSuperCell.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventSuperCell.hh
  ${PROJECT_SOURCE_DIR}/src/event/SLArEventSuperCellSystem.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventSuperCellSystem.hh
  )
target_link_libraries(
  SLArMCEventReadout 
  PUBLIC 
  SLArReadoutSystemConfig ${ROOT_LIBRARIES})
ROOT_GENERATE_DICTIONARY(G__SLArMCEventReadout
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventPhotonHit.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventTile.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventMegatile.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventReadoutTileSystem.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventSuperCell.hh
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventSuperCellSystem.hh
  MODULE SLArMCEventReadout
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/event/SLArEventReadoutLinkDef.h
  )

# Master MC EVENT
add_library(SLArMCEvent 
  SHARED
  ${PROJECT_SOURCE_DIR}/src/event/SLArMCEvent.cc
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCEvent.hh
  )
target_link_libraries(SLArMCEvent 
                      PUBLIC
                      ${ROOT_LIBRARIES} 
                      SLArMCPrimaryInfo
                      SLArMCEventReadout)

ROOT_GENERATE_DICTIONARY(G__SLArMCEvent
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCEvent.hh
  MODULE SLArMCEvent
  LINKDEF
  ${PROJECT_SOURCE_DIR}/include/event/SLArMCEventLinkDef.h
  )

#----------------------------------------------------------------------------
# Install instructions

set(G4SOLAr_event_libs
  SLArMCEvent
  SLArMCPrimaryInfo
  SLArMCEventReadout
  )

set(G4SOLAr_event_resources 
  ${CMAKE_CURRENT_BINARY_DIR}/G__SLArMCPrimaryInfo.cxx 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCPrimaryInfo.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCPrimaryInfo_rdict.pcm
  #
  ${CMAKE_CURRENT_BINARY_DIR}/G__SLArMCEvent.cxx 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCEvent.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCEvent_rdict.pcm
  #
  ${CMAKE_CURRENT_BINARY_DIR}/G__SLArMCEventReadout.cxx 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCEventReadout.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArMCEventReadout_rdict.pcm
  )

foreach(lib IN LISTS G4SOLAr_event_libs)
  install(TARGETS ${lib}
    EXPORT                      G4SOLArTargets
    LIBRARY DESTINATION        "${G4SOLAR_LIB_DIR}" 
    RUNTIME DESTINATION        "${G4SOLAR_LIB_DIR}")
endforeach()

install(FILES ${G4SOLAr_event_resources} DESTINATION "${G4SOLAR_LIB_DIR}")
