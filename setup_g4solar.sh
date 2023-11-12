#!/usr/bin/env bash


######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : setup_g4solar.sh
# @created     : Wednesday Oct 05, 2022 02:25:23 CDT
#
# @description : Setup environment for G4SOLAr
######################################################################

printf "%s\n" '-----------------------------------------'
printf "G4SOLAr Environment Setup - v0.2 2023-04-25\n\n"

printf "setup cmake v3_23_1\n"
setup cmake v3_23_1
printf "setup root v6_22_08d -qe20:p392:prof\n"
setup root v6_22_08d -qe20:p392:prof
printf "setup geant4 v4_11_1_p01ba -qe20:prof\n"
setup geant4 v4_11_1_p01ba -qe20:prof
# export the geant4 installation directory path tothe variable  GEANT4_DIR
export GEANT4_DIR="/cvmfs/larsoft.opensciencegrid.org/products/geant4/v4_11_1_p01ba/Linux64bit+3.10-2.17-e20-prof/bin"
printf "setup gsl v2_6a\n"
setup gsl v2_6a
printf "setup clhep v2_4_5_3a -qe20:prof"
setup clhep v2_4_5_3a -qe20:prof

printf "\nDONE\n"
printf "%s\n" '-----------------------------------------'


