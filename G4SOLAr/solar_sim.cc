//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file solar_sim.cc
/// \brief Main program of the solar_sim framework
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
// Description: Test of Continuous Process G4Cerenkov
//              and RestDiscrete Process G4Scintillation
//              -- Generation Cerenkov Photons --
//              -- Generation Scintillation Photons --
//              -- Transport of optical Photons --
// Version:     5.0
// Created:     1996-04-30
// Author:      Juliet Armstrong
// mail:        gum@triumf.ca
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <getopt.h>
#include "G4Types.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"

#include "SLArUserPath.hh"
#include "SLArAnalysisManager.hh"
#include "SLArPhysicsList.hh"
#include "SLArDetectorConstruction.hh"
#include "SLArActionInitialization.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    fprintf(stderr, " solar_sim\t[-m/--macro macro_file]]\n");
    fprintf(stderr, " \t\t[-o/--output output_file_name]\n");
    fprintf(stderr, " \t\t[-u/--session session]\n");
    fprintf(stderr, " \t\t[-r/--seed user_seed]\n");
    fprintf(stderr, " \t\t[-g/--geometry geometry_cfg_file]\n");
    fprintf(stderr, " \t\t[-p/--materials material_db_file]\n");
    fprintf(stderr, " \t\t[-h/--help print usage]\n");
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{

  G4String macro;
  G4String session;
  G4String output = ""; 
  G4String geometry_file = "./assets/geometry/geometry.json"; 
  G4String material_file = "./assets/materials/materials_db.json"; 

#ifdef G4MULTITHREADED
  G4int nThreads = 0;
#endif

  G4long myseed = 345354;
  const char* short_opts = "m:o:u:t:r:g:p:h";
  static struct option long_opts[9] = 
  {
    {"macro", required_argument, 0, 'm'}, 
    {"output", required_argument, 0, 'u'}, 
    {"session", required_argument, 0, 'u'}, 
    {"threads", required_argument, 0, 't'}, 
    {"seed", required_argument, 0, 'r'}, 
    {"geometry", required_argument, 0, 'g'}, 
    {"materials", required_argument, 0, 'p'},
    {"help", no_argument, 0, 'h'}, 
    {nullptr, no_argument, nullptr, 0}
  };

  int c, option_index; 

  while ( (c = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1) {
    switch(c) {
      case 'm' : 
      {
        macro = optarg;
        printf("solar_sim config macro: %s\n", macro.c_str());
        break;
      };
      case 'o' : {
        output = optarg;
        printf("solar_sim output file: %s\n", output.c_str());
        break;
     };
      case 'u' : 
      {
        session = optarg; 
        printf("solar_sim session: %s\n", session.c_str());
        break;
      };
      case 'r':
      {
        myseed = std::atoi(optarg); 
        printf("solar_sim seed: %lu\n", myseed);
        break;
      }; 
      case 'g':
      {
        geometry_file = optarg; 
        printf("solar_sim geometry configuration file: %s\n", geometry_file.c_str());
        break;
      };
      case 'p' : 
      {
        material_file = optarg; 
        printf("solar_sim material database: %s\n", material_file.c_str());
        break;
      };
      case 'h' : 
      {
        PrintUsage(); 
        return 4;
        break;
      };
      case '?' : 
      {
        printf("solar_sim error: unknown flag %c\n", optopt);
        PrintUsage(); 
        return 4;
        break;
      }; 
#ifdef G4MULTITHREADED
      case 't':
      {
        nThreads = std::atoi(optarg); 
        printf("solar_sim running on %i threads\n", nThreads);
        break;
      };
#endif
    }
  }

  // Instantiate G4UIExecutive if interactive mode
  G4UIExecutive* ui = nullptr;
  if ( macro.size() == 0 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  // force just 1 thread
  nThreads = 1;
  G4MTRunManager * runManager = new G4MTRunManager;
  if ( nThreads > 0 ) runManager->SetNumberOfThreads(nThreads);
#else
  G4RunManager * runManager = new G4RunManager;
#endif

  // Seed the random number generator manually
  G4Random::setTheSeed(myseed);

  G4String physName = "FTFP_BERT_HP";

  // Set mandatory initialization classes
  //
  // Detector construction
  printf("Creating Detector Construction...\n");
  runManager-> SetUserInitialization(new SLArDetectorConstruction(geometry_file, material_file));
  // Physics list
  printf("Creating Phiscs Lists...\n");
  runManager-> SetUserInitialization(new SLArPhysicsList(physName));
  // User action initialization
  printf("Creating User Action...\n");
  runManager->SetUserInitialization (new SLArActionInitialization());

  // Initialize G4 kernel
  //
  printf("RunManager initialization...\n");
  runManager->Initialize();

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  printf("visManager initialization...\n");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  //

  if ( macro.size() ) {
    // Batch mode
    if (!output.empty()) {
      G4String command = "/SLAr/manager/SetOutputName "; 
      UImanager->ApplyCommand(command+output); 
    }
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else // Define UI session for interactive mode
  {
    UImanager->ApplyCommand("/control/execute vis.mac");
    if (ui->IsGUI())
      UImanager->ApplyCommand("/control/execute gui.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  delete visManager;
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
