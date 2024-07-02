######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : venerdì mag 06, 2022 23:01:03 CEST
######################################################################

add_library(SLArIonAndScint
  SHARED
  ${PROJECT_SOURCE_DIR}/src/physics/SLArLArProperties.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArLArProperties.hh
  ${PROJECT_SOURCE_DIR}/src/physics/SLArIonAndScintModel.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArIonAndScintModel.h
  ${PROJECT_SOURCE_DIR}/src/physics/SLArIonAndScintSeparate.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArIonAndScintSeparate.h
  ${PROJECT_SOURCE_DIR}/src/physics/SLArIonAndScintLArQL.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArIonAndScintLArQL.h
  )
target_link_libraries(SLArIonAndScint ROOT::Core)

add_library(SLArScintillation
  SHARED
  ${PROJECT_SOURCE_DIR}/src/physics/SLArScintillation.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArScintillation.h
  #${PROJECT_SOURCE_DIR}/src/physics/SLArIonAndScintLArQL.cc
  #${PROJECT_SOURCE_DIR}/include/physics/SLArIonAndScintLArQL.h
  #${PROJECT_SOURCE_DIR}/src/physics/SLArIonAndScintSeparate.cc
  #${PROJECT_SOURCE_DIR}/include/physics/SLArIonAndScintSeparate.h
  #${PROJECT_SOURCE_DIR}/src/physics/SLArIonAndScintModel.cc
  #${PROJECT_SOURCE_DIR}/include/physics/SLArIonAndScintModel.h
  ${PROJECT_SOURCE_DIR}/src/physics/SLArElectronDrift.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArElectronDrift.hh
  )
add_dependencies(SLArScintillation SLArMCEventReadout SLArReadoutSystemConfig)
target_link_libraries(SLArScintillation SLArIonAndScint ${Geant4_LIBRARIES} SLArMCEventReadout SLArReadoutSystemConfig)

#----------------------------------------------------------------------------
# Install instructions


install(TARGETS SLArScintillation SLArIonAndScint
  EXPORT                      G4SOLArTargets
  LIBRARY DESTINATION        "${G4SOLAR_LIB_DIR}"
  RUNTIME DESTINATION        "${G4SOLAR_LIB_DIR}")
