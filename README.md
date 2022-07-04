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

- **Core:** `Geant4`, `ROOT` (possibly compiled from source *with the same C++ std*)
  and respective dependencies (`cmake`, `g++`, `gcc`)
- **Generators:** `G4SOLAr` integrates some external events generators that
  are relevant for the physics goal of the project. 
  * **MARLEY**: Low-energy neutrino interactions in LAr
  * **BxDecay0**: Generic radioactive decay generator, with the possibility 
    of producing neutrinoless *ββ*-decay final states.
    
  At the preset state of the development, these two packages have to be installed
  manually by the user 
  (see [Event Generators installation and configuration](README_GENERATORS.md)). 
  
## Build the project (sanity check)

### Step 1 - MARLEY check
Verify that the environmental variable `MARLEY` is defined and properly set.

`MARLEY` comes with a useful setup script that can be sourced in your `.bashrc`
file and set up an environmental variable with the path to the MARLEY source code.
You can check out if this variable is properly set by executing 
```bash
$ echo $MARLEY
```
If the above command will return the path to the MARLEY directory, then you can 
go to step 2. Otherwise, run the setup script manually. 

### Step 2 - Build
Build the project
```bash
$ git clone https://github.com/dguff/G4SOLAr.git
$ cd G4SOLAr
$ mkdir build install && cd build 
$ cmake -DCMAKE_PREFIX_PATH=/path/to/bxdecay0/install -DCMAKE_INSTALL_PREFIX=../install ../G4SOLAr
$ make
$ make install
```

### Step 3 - Run

It is possible to run the simulation directly from the build folder, but it
is advised to add the build directory to the executable PATH to be able to run 
the simulation more flexibly on your machine
```bash
$ cd install
$ export PATH=${PWD}:${PATH}
```

The run can be configured via macro files. A collection of examples can 
be found in the `macros/` folder. The commands defined in the messenger
classes are briefly commented in the macro files. 

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
