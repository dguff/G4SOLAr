########################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists.txt
# @created     : Tuesday Oct 18, 2022 11:31:06 CEST
# @description : Analysis libraries to implement a visibility map 
#                based on the semi-analytical model of light propagation
########################################################################

#add_library(SLArLightPropagation SHARED
#  ${G4S_ANALYSIS_SRC_DIR}/SLArLightPropagationModel.cpp
#  ${G4S_ANALYSIS_INC_DIR}/SLArLightPropagationModel.hh
#  ${G4S_ANALYSIS_INC_DIR}/SLArLightPropagationPars.hpp
#  )
#
#target_link_libraries(SLArLightPropagation PUBLIC ${ROOT_LIBRARIES})
#target_link_libraries(SLArLightPropagation PUBLIC G4SOLAr::SLArReadoutSystemConfig)
#
#ROOT_GENERATE_DICTIONARY(G__SLArLightPropagation
#  ${G4S_ANALYSIS_INC_DIR}/SLArLightPropagationModel.hh
#  MODULE SLArLightPropagation
#  LINKDEF ${G4S_ANALYSIS_INC_DIR}/SLArLightPropagationLinkDef.h)

add_library(SLArQEvent SHARED
  ${G4S_ANALYSIS_SRC_DIR}/SLArQCluster.cc
  ${G4S_ANALYSIS_INC_DIR}/SLArQCluster.hh
  ${G4S_ANALYSIS_SRC_DIR}/SLArQEventReadout.cc
  ${G4S_ANALYSIS_INC_DIR}/SLArQEventReadout.hh
  ${G4S_ANALYSIS_SRC_DIR}/SLArQEventAnalysis.cc
  ${G4S_ANALYSIS_INC_DIR}/SLArQEventAnalysis.hh
  )
target_link_libraries(SLArQEvent PUBLIC ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__SLArQEvent
  ${G4S_ANALYSIS_INC_DIR}/SLArQCluster.hh
  ${G4S_ANALYSIS_INC_DIR}/SLArQEventReadout.hh
  ${G4S_ANALYSIS_INC_DIR}/SLArQEventAnalysis.hh
  MODULE SLArQEvent
  LINKDEF ${G4S_ANALYSIS_INC_DIR}/SLArQEventReadoutLinkDef.h)

#----------------------------------------------------------------------
# Install instructions

set(G4SOLAr_analysis_libs
  #SLArLightPropagation
  SLArQEvent
  )

set(G4SOLAr_analysis_resources 
  #${CMAKE_CURRENT_BINARY_DIR}/G__SLArLightPropagation.cxx
  #${CMAKE_CURRENT_BINARY_DIR}/libSLArLightPropagation.rootmap 
  #${CMAKE_CURRENT_BINARY_DIR}/libSLArLightPropagation_rdict.pcm

  ${CMAKE_CURRENT_BINARY_DIR}/G__SLArQEvent.cxx
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArQEvent.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArQEvent_rdict.pcm
  )

foreach(lib IN LISTS G4SOLAr_analysis_libs)
  install(TARGETS ${lib}
    EXPORT                      G4SAnalysisTargets
    LIBRARY DESTINATION        "${G4S_ANALYSIS_LIB_DIR}" 
    RUNTIME DESTINATION        "${G4S_ANALYSIS_LIB_DIR}")
endforeach()

install(FILES ${G4SOLAr_analysis_resources} DESTINATION "${G4S_ANALYSIS_LIB_DIR}")

