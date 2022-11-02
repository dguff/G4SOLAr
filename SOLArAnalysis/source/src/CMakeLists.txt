########################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists.txt
# @created     : Tuesday Oct 18, 2022 11:31:06 CEST
# @description : Analysis libraries to implement a visibility map 
#                based on the semi-analytical model of light propagation
########################################################################

add_library(SLArLightPropagation SHARED
  ${G4S_ANALYSIS_SRC_DIR}/SLArLightPropagationModel.cpp
  ${G4S_ANALYSIS_INC_DIR}/SLArLightPropagationModel.hh
  ${G4S_ANALYSIS_INC_DIR}/SLArLightPropagationPars.hpp
  )

target_link_libraries(SLArLightPropagation PRIVATE ${ROOT_LIBRARIES})
target_link_libraries(SLArLightPropagation PRIVATE G4SOLAr::SLArReadoutSystemConfig)

ROOT_GENERATE_DICTIONARY(G__SLArLightPropagation
  ${G4S_ANALYSIS_INC_DIR}/SLArLightPropagationModel.hh
  MODULE SLArLightPropagation
  LINKDEF ${G4S_ANALYSIS_INC_DIR}/SLArLightPropagationLinkDef.h)

#----------------------------------------------------------------------
# Install instructions

set(G4SOLAr_analysis_libs
  SLArLightPropagation
  )

set(G4SOLAr_analysis_resources 
  ${CMAKE_CURRENT_BINARY_DIR}/G__SLArLightPropagation.cxx
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArLightPropagation.rootmap 
  ${CMAKE_CURRENT_BINARY_DIR}/libSLArLightPropagation_rdict.pcm
  )

foreach(lib IN LISTS G4SOLAr_analysis_libs)
  install(TARGETS ${lib}
    EXPORT                      G4SAnalysisTargets
    LIBRARY DESTINATION        "${G4S_ANALYSIS_LIB_DIR}" 
    RUNTIME DESTINATION        "${G4S_ANALYSIS_LIB_DIR}")
endforeach()

install(FILES ${G4SOLAr_analysis_resources} DESTINATION "${G4S_ANALYSIS_LIB_DIR}")

