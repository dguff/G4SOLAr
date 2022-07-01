# G4SOLAr external event generators

`G4SOLAr` integrates the [MARLEY](https://www.marleygen.org/)
and [BxDecay0](https://github.com/BxCppDev/bxdecay0) 
external generators 
for producing low-energy neutrino interactions and radioactive decays.
Both these packages are **REQUIRED** to build and run `G4SOLAr`.

Detailed installation instructions can be found in the respective 
repositories/websites.

## MARLEY
Clone and build the package 
```bash
$ git clone https://github.com/MARLEY-MC/marley.git
$ cd marley/build
$ make 
```
Source the setup script `setup_marley.sh` to define the `MARLEY` environmental
variable and add the build folder to the executable PATH. 

## BxDecay0
Clone and build the package
```bash
$ git clone https://github.com/BxCppDev/bxdecay0.git
$ cd bxdecay0
$ mkdir build && mkdir install
$ cd build
$ cmake -DCMAKE_INSTALL_PREFIX=../install -DBXDECAY0_WITH_GEANT4_EXTENSION=ON -DGeant4_DIR=/path/to/geant4/installation
$ make && make install
```


