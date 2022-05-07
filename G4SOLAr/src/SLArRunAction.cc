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
// $Id: SLArRunAction.cc 74204 2013-10-01 07:04:43Z ihrivnac $
//
/// \file SLArRunAction.cc
/// \brief Implementation of the SLArRunAction class

#include "SLArAnalysisManager.hh"
#include "SLArRunAction.hh"
#include "SLArAnalysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// In case you want to store data in an array see ex. b4/5
SLArRunAction::SLArRunAction()
 : G4UserRunAction()
{ 
  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in SLArAnalysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Default settings
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
  analysisManager->SetFileName("./output/testCell_out.root");

  // Book histograms, ntuple
  //
  
  // Creating 3D histograms
  analysisManager
    ->CreateH3("h3LAPPD","LAPPD hits", 
                40, -10, +10,
                40, -10, +10, 
                50,   0, +50,
                "mm","mm",  "ns"); // h3 Id = 0
  
  // Creating 2D histograms
  analysisManager                                                
    ->CreateH2("LAPPDXY","LAPPD X vs Y",           // h2 Id = 0
               50, -10., 10, 50, -10., 10.); 
  analysisManager                                                
    ->CreateH2("PMTsXY","PMTs X vs Y",             // h2 Id = 1
               50, -10., 10, 50, -10., 10.); 

  //// Creating ntuple
  analysisManager->CreateNtuple("PMThits", "PMTs hits");
  analysisManager->CreateNtupleIColumn("EvNr");
  analysisManager->CreateNtupleIColumn("PhType");
  analysisManager->CreateNtupleIColumn("Idx");
  analysisManager->CreateNtupleDColumn("Time");
  analysisManager->CreateNtupleDColumn("GloX");
  analysisManager->CreateNtupleDColumn("GloY");
  analysisManager->CreateNtupleDColumn("PhEne");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("LAPPDhits", "LAPPDhits");
  analysisManager->CreateNtupleIColumn("EvNr");
  analysisManager->CreateNtupleIColumn("PhType");
  analysisManager->CreateNtupleDColumn("Time");
  analysisManager->CreateNtupleDColumn("GloX");
  analysisManager->CreateNtupleDColumn("GloY");
  analysisManager->CreateNtupleDColumn("PhEne");
  analysisManager->FinishNtuple();

  // Create custom TestCell Analysis Manager
  SLArAnalysisManager* anamgr = SLArAnalysisManager::Instance();
  anamgr->FakeAccess();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArRunAction::~SLArRunAction()
{
  delete G4AnalysisManager::Instance();  
  delete SLArAnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArRunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Open an output file 
  // The default file name is set in SLArRunAction::SLArRunAction(),
  // it can be overwritten in a macro
  analysisManager->OpenFile();

  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance(); 
  SLArAnaMgr->CreateFileStructure();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArRunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // save histograms & ntuple
  //
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

  SLArAnalysisManager* tcAnaMgr = SLArAnalysisManager::Instance();
  //tcAnaMgr->WriteSysCfg();
  tcAnaMgr->Save();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
