# SOLAr-sim

**SOLAr-sim** (sometimes also called `G4SOLAr`) implements a Geant4-based 
Monte Carlo simulation of a generic LAr TPC
for studying low-energy events in the context of the [SOLAr project](https://solar-project.web.cern.ch/). 

## Disclaimer
This code has never been properly tested, but there are some reasonable 
expectations for it to work on a Linux machine. While offering my sincerest
sympathies for your struggles, I cannot bear the responsibility for other 
developers' tears. 

## Prerequisites

- **Core:** `Geant4` `v11.0` and newer, **compiled without `MULTI_THREAD` support**, 
  `ROOT` (possibly compiled from source)
  and respective dependencies (`cmake`, `g++`, `gcc`)
- **Generators:** `SOLAr-sim` integrates some external events generators that
  are relevant for the physics goal of the project. 
  * **MARLEY**: Low-energy neutrino interactions in LAr
  * **BxDecay0**: Generic radioactive decay generator, with the possibility 
  of producing neutrinoless *ββ*-decay final states.
  * **CRY**: [optional] Cosmic-ray shower generator
  * **RadSrc**: [optional] Generator reproducing the gamma ray spectrum of composite sources 

- **Utilities**: `SOLAr-sim` uses the `RapidJSON` package to parse configuration 
  files formatted according to the `json` standard. 
    
  The `SOLAr-sim` package includes a convenient script 
  to automatically download, build and install the required external dependencies
  (see [External dependencies installation and configuration](G4SOLAr/extern/README_EXTERNALS.md)). 
  Optional dependencies such as CRY and RadRsc should be installed by the user.
  
## Download and build the project

Please note that the procedure to build the code have some subtle differences 
whether one is installing the simulation on a generic machine
such as the CERN computing environment. Here we describe the installation for a 
generic Unix system.

### Step 1 - Download the project from GitHub and install dependencies
(If you prefer ssh key authentication change the repository url accordingly)
```bash
[dguff  version]$ git clone https://github.com/dguff/G4SOLAr.git
[SoLAr  version]$ git clone https://github.com/SoLAr-Neutrinos/SOLAr-sim.git
```
If the project dependencies are not yet installed, follow the instructions
on [this page](./G4SOLA/extern/README_EXTERNALS.md).

### Step 2 - Build and install
Create a build and install directory, then build and install the project
```bash
$ mkdir build install && cd build 
$ cmake -DCMAKE_INSTALL_PREFIX=../install [opts...] ../G4SOLAr
$ make -jN
$ make install
```
The project will search for the external dependencies in the 
`G4SOLAR_EXT_DIR` (by default set to `G4SOLAr/extern/install`). You can 
specify a specific installation directory by setting it in the `cmake`
command line (`-DG4SOLAR_EXT_DIR=/my/g4solar_ext/path`). 

### Step 3 - Run SOLAr-sim

It is possible to run the simulation directly from the installation folder, but it
is advised to add the build directory to the executable PATH to be able to run 
the simulation more flexibly on your machine
```bash
$ cd install
$ export PATH=${PWD}:${PATH}
```

The `solar_sim` executable can take the following inputs:
```bash
solar_sim      
               [-g | --geometry geometry_cfg_file]      #<< Geometry description
               [-m | --macro macro_file]                #<< Geant4 mac file
               [-r | --seed user_seed]                  #<< User defined seed
               [-p | --materials material_db_file]      #<< Material definition table
               [-x | --generator generator_config_file]
               [-l | --physics_list modular_phys_list]  #<< set basic physics list (default is FTFP_BERT_HP)]
               [-o | --output output_file]              #<< set output file name
               [-d | --output_dir output_dir]           #<< set output directory
               [-b | --bias particle <process_list> bias_factor] #<< apply a scaling factor to a given particle (process) cross section
               [-h | --help print usage]    
```

The first input (`-m`, `--macro`) is the configuration file for the 
Geant4 run. There one can specify the type of generated events, 
their number, the output file, etc. A collection of examples can 
be found in the `macros/` folder. The commands defined in the messenger
classes are briefly commented in the macro files. 

The second input (`-s`, `--seed`) is the random engine seed provided by the user.

The third input (`-g`, `--geometry`) is the `json` configuration file describing
the dimensions of the detector and of all the sub-systems. 

Finally, the fourth input (`-p`, `--materials`) is a json table containing
the definitions of all materials used in the simulation. 

If no material table is provided, the simulation will take 
by default `assets/materials/materials_db.json`. 

## Interpreting the output

The output file consists in a ROOT Tree containing the full development of 
an event at the step-level and the information from the anode and photon detection 
system. These three distinct set of information are stored into a 
`SLArMCEvent` object, which in turn contains a vector of `SLArMCPrimaryInfo`
(one for each particle in the initial state), a PDS event object 
(`SLArEventReadoutTileSystem`) and an anode event object for each anode module 
in the detector. 

### Primary info

In addition to its own trajectory, each "primary" carries 
the trajectories of all secondary particles associated with the primary track.
The points of each trajectory are defined by their spatial coordinates and by 
the energy deposited in the step. 

The "hierarchical" structure of the primary information object is represented 
in the figure below (Fig. 1)

| ![SLArMCPrimaryInfo diagram](./docs/figures/SLArMCPrimaryInfo.png)           |
| :--:                                                                         |
| **Fig. 1** Box diagram showing the structure of a SLArMCPrimaryInfo object   |


<!--### Photon Detection System Info-->

<!--The photon hits recorded by the photon detection system are recorded in -->
<!--a dedicated event object. In the case of readout tiles, with each tile mounting-->
<!--up to 100 SiPM, the detected photons are grouped together in a single score. -->

<!--TODO: add to the photon hit attributes the identification of the specific SiPM-->
<!--responsible for the detection. -->

<!--Figure 2 schematically represents the structure of the ReadoutTile-based -->
<!--PDS event object.-->

<!--| ![PDS event diagram](./docs/figures/SLArEventPDS.png)                                                |-->
<!--| :--:                                                                                                 |-->
<!--| **Fig. 2** Schematic representation of the event class for the PDS based on the readout tile concept |-->



### SOLAr Event Dictionaries

To be able to access the event information in an interactive ROOT session, 
one should load the shared libraries defining the event and configuration 
objects. 
These libraries are built when compiling `SOLAr-sim` and are installed in 
the `G4SOLAR_INSTALL_DIR/lib` folder. During the installation process, 
a `rootlogon.C` file loading the libraries is created in 
`G4SOLAR_BASE_DIR/SOLArAnalysis`. In the same folder, one can
find the script `test_output.C`, which can serve as an example for accessing
simulated MC event. 

### Running jobs on the grid

#### CERN
See the documentation of [**lx-solar**](https://github.com/SoLAr-Neutrinos/lx-solar), 
a set of scripts and utilities setting the SoLAr environment on lxplus.

#### FNAL
You can run SOLAr-sim jobs on the grid with

```
cd grid
source setup_g4solar_grid.sh
python jobsub.py [options]
```

You can see all the available options with `python jobsub.py -h`

`jobsub.py` uses a template macro to run the jobs. The current available template is `template_g4solar_electrons.mac`.
To run other types of events, more templates will have to be added. 

TODO: add a full macro file writer



