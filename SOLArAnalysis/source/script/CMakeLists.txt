######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : martedì ott 25, 2022 17:23:57 CEST
######################################################################

list(APPEND G4S_ANALYSIS_RPATH
  "${G4SOLAR_LIB_DIR}"
  "${G4S_ANALYSIS_LIB_DIR}"
  )
set(CMAKE_INSTALL_RPATH "${G4S_ANALYSIS_RPATH}")
message(STATUS "G4S_ANALYSIS_RPATH: ${CMAKE_INSTALL_RPATH}")

add_executable(test_output test_output.cc)
target_link_libraries(test_output PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(test_output PUBLIC
  G4SOLAr::SLArReadoutSystemConfig 
  G4SOLAr::SLArMCEvent
  G4SOLAr::SLArMCEventReadout
  G4SOLAr::SLArMCPrimaryInfo)

add_executable(coverage_study coverage_study.cc)
target_link_libraries(coverage_study PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(coverage_study PUBLIC
  G4SOLAr::SLArReadoutSystemConfig 
  G4SOLAr::SLArMCEvent
  G4SOLAr::SLArMCEventReadout
  G4SOLAr::SLArMCPrimaryInfo)

add_executable(build_vis_map build_vis_map.cc)
target_link_libraries(build_vis_map PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(build_vis_map PUBLIC ${Geant4_LIBRARIES})
target_link_libraries(build_vis_map PUBLIC 
  G4SOLAr::SLArReadoutSystemConfig 
  G4SOLAr::SLArMCEvent
  G4SOLAr::SLArMCEventReadout
  G4SOLAr::SLArMCPrimaryInfo)
target_link_libraries(build_vis_map PUBLIC SLArLightPropagation)

list(APPEND analysis_executables 
  test_output 
  build_vis_map
  coverage_study
  )

install(TARGETS test_output
  LIBRARY DESTINATION "${G4S_ANALYSIS_LIB_DIR}"
  RUNTIME DESTINATION "${G4S_ANALYSIS_BIN_DIR}"
  )

install(TARGETS coverage_study
  LIBRARY DESTINATION "${G4S_ANALYSIS_LIB_DIR}"
  RUNTIME DESTINATION "${G4S_ANALYSIS_BIN_DIR}"
  )

install(TARGETS build_vis_map
  LIBRARY DESTINATION "${G4S_ANALYSIS_BIN_DIR}"
  RUNTIME DESTINATION "${G4S_ANALYSIS_BIN_DIR}"
  )


