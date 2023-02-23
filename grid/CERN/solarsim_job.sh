#!/usr/bin/bash

echo "SoLAr-sim job execution script - v0.1.0"
echo "Brought to you by the SoLAr collaboration"

echo "Setup working node environment..."
source /cvmfs/sft.cern.ch/lcg/views/LCG_geant4ext20211109/x86_64-centos7-gcc10-opt/setup.sh
source /cvmfs/geant4.cern.ch/geant4/11.0.p03/x86_64-centos7-gcc10-optdeb/CMake-setup.sh
source /cvmfs/geant4.cern.ch/geant4/11.0.p03/x86_64-centos7-gcc10-optdeb/bin/geant4.sh

SOLARSIM_INSTALL_DIR="/afs/cern.ch/user/g/guffantd/SOLAr-sim/install"
export PATH=${PATH}:${SOLARSIM_INSTALL_DIR}

ARGLIST=$1

SEED=$( sed -nE 's/.*--seed\s+([0-9]+).*/\1/p' ${ARGLIST} )
echo "Job seed is ${SEED}"

FILENAME=$( sed -nE 's/.*--output\s+([^[:space:]]+).*/\1/p' ${ARGLIST} )
EOS_FILENAME=$( basename ${FILENAME} .root )
EOS_FILENAME="${EOS_FILENAME}_${SEED}.root"
echo "Output file name is ${EOS_FILENAME}"

echo "Launching process execution..."
solar_sim $( cat ${ARGLIST} )

echo "Moving output file to EOS filesystem"
mv ${FILENAME} "/eos/home-g/guffantd/test_sub/${EOS_FILENAME}" 

echo "Done! Thanks for running!"
