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
                      SLArMCPrimaryInfo)


