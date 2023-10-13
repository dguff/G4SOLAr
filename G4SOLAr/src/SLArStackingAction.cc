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
#include "SLArPrimaryGeneratorAction.hh"
#include "SLArUserTrackInformation.hh"

#include "G4VProcess.hh"
#include "G4RunManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4ios.hh"
#include <vector>

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
  G4ClassificationOfNewTrack kClassification = fUrgent; 

  if(aTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
    // check it the track already owns a user info 
    if (aTrack->GetUserInformation()) {
      //printf("Track ID %i already has User Information\n", aTrack->GetTrackID());
      return kClassification;
    }
    else {
      //printf("Track ID %i is a new one!\n", aTrack->GetTrackID());
      auto SLArAnaMgr = SLArAnalysisManager::Instance(); 
      G4int parentID = 0; 
      if (aTrack->GetParentID() == 0) { // this is a primary
        fEventAction->RegisterNewTrackPID(aTrack->GetTrackID(), aTrack->GetTrackID()); 
        parentID = aTrack->GetTrackID(); 
        // fix track ID in primary output object
        auto& primaries = SLArAnaMgr->GetEvent()->GetPrimaries();
        for (auto &primaryInfo : primaries) {
          if (fabs(aTrack->GetMomentum().x() - primaryInfo->GetMomentum()[0]) < 1e-6 &&
              fabs(aTrack->GetMomentum().y() - primaryInfo->GetMomentum()[1]) < 1e-6 &&
              fabs(aTrack->GetMomentum().z() - primaryInfo->GetMomentum()[2]) < 1e-6) {
            //printf("This is a primary: Corrsponding primary info found (%i)\n", primaryInfo.GetTrackID());
            primaryInfo->SetTrackID(aTrack->GetTrackID()); 
            break;
          }
        }
      } else {
        //printf("Not a primary, recording parent id\n");
        fEventAction->RegisterNewTrackPID(aTrack->GetTrackID(), aTrack->GetParentID()); 
        parentID = aTrack->GetParentID(); 
      }

      const char* particleName = aTrack->GetParticleDefinition()->GetParticleName().data(); 
      const char* creatorProc  = "PrimaryGenerator"; 
      if (aTrack->GetCreatorProcess()) {
        creatorProc = aTrack->GetCreatorProcess()->GetProcessName(); 
      }
      
      //printf("creating trajectory...\n");
      std::unique_ptr<SLArEventTrajectory> trajectory = std::make_unique<SLArEventTrajectory>();
      //SLArEventTrajectory* trajectory = new SLArEventTrajectory();
      trajectory->SetTrackID( aTrack->GetTrackID() ); 
      trajectory->SetParentID(aTrack->GetParentID()); 
      trajectory->SetParticleName( particleName );
      trajectory->SetPDGID( aTrack->GetDynamicParticle()->GetPDGcode() ); 
      trajectory->SetCreatorProcess( creatorProc ); 
      trajectory->SetTime( aTrack->GetGlobalTime() ); 
      trajectory->SetWeight(aTrack->GetWeight()); 
      

      trajectory->SetInitKineticEne( aTrack->GetKineticEnergy() ); 
      auto vertex_momentum = aTrack->GetMomentumDirection();
      trajectory->SetInitMomentum( TVector3(
            vertex_momentum.x(), vertex_momentum.y(), vertex_momentum.z() ) );
      //printf("Looking for ancestor...\n");
      G4int ancestor_id = fEventAction->FindAncestorID( parentID ); 
      //printf("found\n");

      //SLArMCPrimaryInfoUniquePtr* ancestor = nullptr; 
      SLArMCPrimaryInfoUniquePtr* ancestor = nullptr; 
      auto& primaries = SLArAnaMgr->GetEvent()->GetPrimaries();
      for (auto &p : primaries) {
        if (p->GetTrackID() == ancestor_id) {
          ancestor = p.get(); 
          break;
        }
      }

#ifdef SLAR_DEBUG
      if (!ancestor) printf("Unable to find corresponding primary particle\n");
#endif

      ancestor->RegisterTrajectory( std::move(trajectory) ); 

      auto trkInfo = new SLArUserTrackInformation( ancestor->GetTrajectories().back().get() ); 

      trkInfo->SetStoreTrajectory(true); 

      aTrack->SetUserInformation( trkInfo ); 
    }
  }
  else 
  { // particle is optical photon
    if(aTrack->GetParentID()>0)
    { // particle is secondary
      SLArAnalysisManager* anaMngr = SLArAnalysisManager::Instance(); 
      SLArMCPrimaryInfoUniquePtr* primary = nullptr; 
      //SLArMCPrimaryInfoPtr* primary = nullptr; 
      auto& primaries = anaMngr->GetEvent()->GetPrimaries();

      int primary_parent_id = fEventAction->FindAncestorID(aTrack->GetParentID()); 
//#ifdef SLAR_DEBUG
      //printf("Primary parent ID %i\n", primary_parent_id);
//#endif
      for (auto &p : primaries) {
        if (p->GetTrackID() == primary_parent_id) {
          primary = p.get(); 
//#ifdef SLAR_DEBUG
          //printf("primary parent found\n");
//#endif
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


      auto generatorAction = 
        (SLArPrimaryGeneratorAction*)G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();  
      if (generatorAction->DoTraceOptPhotons() == false) {
        kClassification = G4ClassificationOfNewTrack::fKill;
      }
    }
  }


  return kClassification;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArStackingAction::NewStage()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArStackingAction::PrepareNewEvent()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
