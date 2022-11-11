######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : venerdì mag 06, 2022 23:01:03 CEST
######################################################################

# SLArMCPrimaryInfo dictionary
add_library(SLArScintillation
  SHARED
  ${PROJECT_SOURCE_DIR}/src/physics/SLArScintillation.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArScintillation.h
  ${PROJECT_SOURCE_DIR}/src/physics/SLArLightYield.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArLightYield.h
  ${PROJECT_SOURCE_DIR}/src/physics/SLArElectronDrift.cc
  ${PROJECT_SOURCE_DIR}/include/physics/SLArElectronDrift.hh
  )
target_link_libraries(SLArScintillation ${GEANT4_LIBRARIES})

#----------------------------------------------------------------------------
# Install instructions


  install(TARGETS SLArScintillation
    LIBRARY DESTINATION        "${G4SOLAR_LIB_DIR}"
    RUNTIME DESTINATION        "${G4SOLAR_LIB_DIR}")
