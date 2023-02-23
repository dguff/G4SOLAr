#!/bin/bash

echo
echo "======== cd to CONDOR_DIR_INPUT ========"
cd $CONDOR_DIR_INPUT

echo
echo "======== ls ========"
ls

echo
echo "======== UNTARRING... ========"
tar xvfz local_install_g4solar.tar.gz -C ./ > /dev/null

echo
echo "======== Done untarring. ls ========"
ls

echo "======== SETUP G4, ROOT, ETC ========"
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
source setup_g4solar.sh
export PATH=${PWD}:${PATH}

echo
echo "======== UPDATE OUTPUT FILE IN PARAMETER FILE ========"
OUTFILE="g4solar_${PROCESS}.root"
sed -i 's/\${output_filename}/'$OUTFILE'/g' parameter_g4solar_grid.mac

echo
echo "======== PARAMETER FILE CONTENT ========"
cat parameter_g4solar_grid.mac

echo
echo "======== EXECUTING G4SOLAR ========"
echo "./solar_sim parameter_g4solar_grid.mac"
./solar_sim -m parameter_g4solar_grid.mac

echo
echo "Moving output to CONDOR_DIR_G4SOLAR"

mv *.root $CONDOR_DIR_G4SOLAR


