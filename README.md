# G4SOLAr

**G4SOLAr** implements a Geant4-based Monte Carlo simulation of a LAr TPC
optimized for low-energy events searches. 

## Disclaimer
This code has never been properly tested, but there are some reasonable 
expectations for it to work on a Linux machine. While offering my sincerest
sympathies for your struggles, I cannot bear the responsibility for other 
developers' tears. 

In the source code one may find large sections commented and ``peculiar'' 
choices for some names (such as _Tank_ to indicate the cryostat). 
This is happening because a large part of this code is based on a MC simulation 
I developed for WbLS optical neutrino detector. 
Hopefully, a more appropriate naming convention will gain ground.

## Prerequisites (on a Linux machine)

- **Core:** `Geant4` `v11.0` and newer, **compiled without `MULTI_THREAD` support**, 
  `ROOT` (possibly compiled from source)
  and respective dependencies (`cmake`, `g++`, `gcc`)
- **Generators:** `G4SOLAr` integrates some external events generators that
  are relevant for the physics goal of the project. 
  * **MARLEY**: Low-energy neutrino interactions in LAr
  * **BxDecay0**: Generic radioactive decay generator, with the possibility 
    of producing neutrinoless *ββ*-decay final states.
- **Utilities**: `G4SOLAr` uses the `RapidJSON` package to parse configuration 
  files formatted according to the `json` standard. 
    
  At the preset state of the development, these three packages have to be installed
  manually by the user. The `G4SOLAr` package includes a convenient script 
  to automatically download, build and install the external dependencies
  (see [External dependencies installation and configuration](README_EXTERNALS.md)). 
  
## Download and build the project

The procedure to build the code have some subtle differences wether one is 
installing the simulation on a [generic machine](#installing-g4solar-on-a-generic-machine)
or on a more complex system 
such as the [Fermilab computing environment](#installing-g4solar-on-fermilab-gpvm). 

### Installing G4SOLAr on Fermilab gpvm
#### Step 1 - Download the project from github, setup the environment and install dependencies
```bash
$ git clone https://github.com/dguff/G4SOLAr.git
```
After cloning the repository, setup the needed dependencies that are already 
installed on the FNAL ecosystem by sourcing the `setup_g4solar.sh` script. 
```bash
$ cd G4SOLAr
$ source setup_g4solar.sh
```
If the project dependencies are not yet installed, follow the instructions
on [this page](./README_EXTERNALS.md).


#### Step 2 - Build

Create a build and install directory, then build and install the project
```bash
$ mkdir build install && cd build 
$ cmake -DGeant4_DIR=${GEANT4_DIR} -DCLHEP_EXTERNAL=${CLHEP_INC} -DCMAKE_INSTALL_PREFIX=../install [opts...] ../G4SOLAr
$ make
$ make install
```
The project will search for the external dependencies in the 
`G4SOLAR_EXT_DIR` (by default set to `G4SOLAr/extern/`). You can 
specify a specific installation directory by setting it in the `cmake`
command line (`-DG4SOLAR_EXT_DIR=/my/g4solar_ext/path`). 
Note that the geant4 version installed on `dunegpvm` nodes is compiled 
against an independent installation of `CLHEP`, so one has to specify the 
`CLHEP` include directory when calling `cmake`.

### Installing G4SOLAr on a generic machine
#### Step 1 - Download the project from github and install dependencies
```bash
$ git clone https://github.com/dguff/G4SOLAr.git
```
If the project dependencies are not yet installed, follow the instructions
on [this page](./README_EXTERNALS.md).

#### Step 2 - Build
Create a build and install directory, then build and install the project
```bash
$ mkdir build install && cd build 
$ cmake -DGeant4_DIR=/path/to/geant4/install -DCMAKE_INSTALL_PREFIX=../install [opts...] ../G4SOLAr
$ make
$ make install
```
The project will search for the external dependencies in the 
`G4SOLAR_EXT_DIR` (by default set to `G4SOLAr/extern/`). You can 
specify a specific installation directory by setting it in the `cmake`
command line (`-DG4SOLAR_EXT_DIR=/my/g4solar_ext/path`). 

### Run G4SOLAr

It is possible to run the simulation directly from the installation folder, but it
is advised to add the build directory to the executable PATH to be able to run 
the simulation more flexibly on your machine
```bash
$ cd install
$ export PATH=${PWD}:${PATH}
```

The `solar_sim` executable can take the following inputs:
```bash
solar_sim      [-m/--macro macro_file]            #<< Geant4 mac file
               [-r/--seed user_seed]              #<< User defined seed
               [-g/--geometry geometry_cfg_file]  #<< Geometry description
               [-p/--materials material_db_file]  #<< Material definition table
               [-h/--help print usage]
```

The first input (`-m`, `--macro`) is the configuration file for the 
Geant4 run. There one can specify the type of generated events, 
their number, the output file, etc. A collection of examples can 
be found in the `macros/` folder. The commands defined in the messenger
classes are briefly commented in the macro files. 

The second input (`-s`, `--seed`) is the random engine seed provided by the user.

The third input (`-g`, `--geometry`) is the `json` configuration file describing
the dimensions of the detector and of all the sub-systemd. 

Finally, the fourth input (`-p`, `--materials`) is a json table containing
the definitions of all materials used in the simulation. 

If no geometry or material table are provided, the simulation will take 
by default `G4SOLAr/geometry.json` and `G4SOLAr/materials/materials_db.json`
respectively. 

## Interpreting the output

The output file consists in a ROOT Tree containing the full development of 
an event at the step-level. The event information is stored in the 
`SLArMCEvent` class, which in turn contains a vector of `SLArMCPrimaryInfo`
(one for each particle in the initial state). 

In addition to its own trajectory, each "primary" carries 
the trajectories of all secondary particles associated with the primary track.
The points of each trajectory are defined by their spatial coordinates and by 
the energy deposited in the step. 

To be able to access the event information in an interactive ROOT session, 
one should load the `SLArMCEvent` and `SLArMCPrimaryInfo` shared libraries 
(manually by invoking `gSystem->Load("/path/to/libSLArMCEvent.so")`, or by 
adding the same command to a `rootlogon.C` file). 
The following script shows how to access the step information. 

```C++
void view_trajectories(const char* filename) {
  TFile* file = new TFile(filename); 
  TTree* tree = (TTree*)file->Get("EventTree"); 

  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 

  for (int iev = 0; iev<t->GetEntries(); iev++) {
    tree->GetEntry(iev); 

    auto primaries = ev->GetPrimaries(); 

    size_t ip = 0; 
    for (const auto &p : primaries) {
      int pPDGID = p->GetCode();     // Get primary PDG code 
      int pTrkID = p->GetTrackID();  // Get primary trak id   
      double primary_edep = 0; 

      auto trajectories = p->GetTrajectories(); 
      int itrj = 0;
      for (const auto &trj : trajectories) {
        for (const auto &tp : trj->GetPoints()) {
          double pos_x = tp.fX;     // x coordinate [mm]
          double pos_y = tp.fY;     // y coordinate [mm]
          double pos_z = tp.fZ;     // z coordinate [mm]
          double edep  = tp.fEdep;  // Energy deposited in the step [MeV]
        }
        itrj++;
      } 
      ip++;
    }
  }

  reutrn;
}
```

If you want to get a better idea of the output structure, the source files 
can be found in 
```
G4SOLAr/include/event/*.hh
G4SOLAr/src/event/*.cc
```
