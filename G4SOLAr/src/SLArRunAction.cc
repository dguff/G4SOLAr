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

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// In case you want to store data in an array see ex. b4/5
SLArRunAction::SLArRunAction()
 : G4UserRunAction()
{ 
  // Create custom TestCell Analysis Manager
  SLArAnalysisManager* anamgr = SLArAnalysisManager::Instance();
  anamgr->FakeAccess();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArRunAction::~SLArRunAction()
{
  delete SLArAnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArRunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance(); 
  SLArAnaMgr->CreateFileStructure();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArRunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // save histograms & ntuple
  //
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
  //SLArAnaMgr->WriteSysCfg();
  SLArAnaMgr->Save();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
