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
/// \file SLAr/src/SLArStackingAction.cc
/// \brief Implementation of the SLArStackingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SLArStackingAction.hh"
#include "SLArEventAction.hh"
#include "SLArAnalysisManager.hh"

#include "G4VProcess.hh"
#include "G4RunManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArStackingAction::SLArStackingAction(SLArEventAction* ea)
  : G4UserStackingAction(), fEventAction(ea)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArStackingAction::~SLArStackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack
SLArStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  if(aTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
    if (aTrack->GetParentID() == 0) {
      fEventAction->RegisterNewTrackPID(aTrack->GetTrackID(), aTrack->GetTrackID()); 
    } else {
      fEventAction->RegisterNewTrackPID(aTrack->GetTrackID(), aTrack->GetParentID()); 
    }
  }
  else 
  { // particle is optical photon
    if(aTrack->GetParentID()>0)
    { // particle is secondary
      SLArAnalysisManager* anaMngr = SLArAnalysisManager::Instance(); 
      SLArMCPrimaryInfo* primary = nullptr; 
      auto primaries = anaMngr->GetEvent()->GetPrimaries();

      int primary_parent_id = fEventAction->FindTopParentID(aTrack->GetParentID()); 
#ifdef SLAR_DEBUG
      printf("Primart parent ID %i\n", primary_parent_id);
#endif
      for (auto &p : primaries) {
        if (p->GetTrackID() == primary_parent_id) {
          primary = p; 
#ifdef SLAR_DEBUG
          printf("primary parent found\n");
#endif

          break; 
        }
      }
       
#ifdef SLAR_DEBUG
      if (!primary) printf("Unable to find corresponding primary particle\n");
#endif

      if(aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation") {
        fEventAction->IncPhotonCount_Scnt();
        if (primary) primary->IncrementScintPhotons(); 
      }
      else if(aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov") {
        fEventAction->IncPhotonCount_Cher();
        if (primary) primary->IncrementCherPhotons();
      }
      else if(aTrack->GetCreatorProcess()->GetProcessName() == "WLS") {
        fEventAction->IncPhotonCount_WLS();
      }
#ifdef SLAR_DEBUG
      else 
        printf("SLArStackingAction::ClassifyNewTrack unknown photon creation process %s\n", 
            aTrack->GetCreatorProcess()->GetProcessName().c_str());
#endif
    }
  }
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArStackingAction::NewStage()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArStackingAction::PrepareNewEvent()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
