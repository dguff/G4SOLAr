######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : CMakeLists
# @created     : lunedì giu 27, 2022 13:48:51 CEST
######################################################################

add_executable(process_file process_events.cpp)
target_link_libraries(process_file ${ROOT_LIBRARIES})
target_link_libraries(process_file ${SLArMCEvent} ${SLArMCPrimaryInfo})
target_link_libraries(process_file SLArQReadout SLArQDiffusion SLArQEvReco)

target_compile_definitions(process_file PUBLIC $<$<CONFIG:Debug>:DEBUG>)
