# G4SOLAr

**G4SOLAr** implements a Geant4-based Monte Carlo simulation of a LAr TPC
optimised for low-energy events searches. 

## Disclaimer
This code has never been properly tested 

## Prerequisites (on a Linux machine)

- **Core:** `Geant4`, `ROOT` (possibly compiled from source *with the same C++ std*)
  and respective dependencies (`cmake`, `g++`, `gcc`)
- **Generators:** `G4SOLAr` integrates some external events generators that
  are relevant for the physics goal of the project. 
  * **MARLEY**: Low-energy neutrino interactions in LAr
  * **BxDecay0**: Generic radioactive decay generator, with the possibility 
    of producing neutrinoless *ββ*-decay final states.
    
  At the preset state of the development, these two packages have to be installed
  manually by the user (see Event Generators installation and configuration). 
  
## Build the project (sanity check)

### Step 1
Verify that the environmental variable `MARLEY` is defined and properly set.

`MARLEY` comes with a useful setup script that can be sourced in your `.bashrc`
file and set up an environmental variable with the path to the MARLEY source code.
You can check out if this variable is properly set by executing 
```bash
$ echo $MARLEY
```
If the above command will return the path to the MARLEY directory, then you can 
go to step 2. Otherwise, run the setup script manually. 

### Step 2
Build the project
```bash
$ git clone https://github.com/dguff/G4SOLAr.git
$ cd G4SOLAr
$ mkdir build && cd build 
$ cmake -DCMAKE_PREFIX_PATH=/path/to/bxdecay0/install ../G4SOLAr
$ make
```
