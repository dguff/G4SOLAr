# G4SOLAr external dependencies

`G4SOLAr` integrates a few external prjects that are not part of this 
repository but that are **required** to build and run `G4SOLAr`. 

[MARLEY](https://www.marleygen.org/)
and [BxDecay0](https://github.com/BxCppDev/bxdecay0) 
are two external generators 
for producing low-energy neutrino interactions and radioactive decays
respectively, while [RapidJSON](https://rapidjson.org/) is a fast `json`
parser for C++.

Detailed installation instructions can be found in the respective 
repositories/websites. For installing the dependencies in this 
project we provide a `cmake` script that will download, build and install 
the three packages. 

In order to install the external dependencies follow these steps:

1. Download the `G4SOLAr` package (if you haven't done it yet)
```bash
$ git clone https://github.com/dguff/G4SOLAr.git 
```
If you are working on a Fermilab computing node, source the setup script to 
access the right version of Geant4, GSL, CMake and CLHEP
```bash
$ cd G4SOLAr
$ source setup_g4solar.sh
```
2. Move into the `G4SOLAr/extern/` folder and run the `cmake` script
```bash
$ cd G4SOLAr/G4SOLAr/extern
$ mkdir build && cd build/
$ cmake -DCMAKE_INSTALL_PREFIX=.. -DGeant4_DIR=/path/to/geant4/installation ..
& make 
```
Note that if you setup Geant4 using the setup script on the Fermilab node, 
the Geant4 installation directory is stored in the `GEANT4_DIR` variable, so
```bash
$ cmake -DCMAKE_INSTALL_PREFIX=.. -DGeant4_DIR=${GEANT4_DIR} ..
```

After these commands, the directory indicated by `CMAKE_INSTALL_PREFIX ` 
(in this case `G4SOLAr/extern/`) should contain the 
installation folders of the three above mentioned packages. 
