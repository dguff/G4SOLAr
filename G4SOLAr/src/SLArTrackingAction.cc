/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArTrackingAction.cc
 * @created     : martedì ago 09, 2022 22:04:56 CEST
 */

#include "SLArAnalysisManager.hh"

#include "SLArTrajectory.hh"
#include "SLArTrackingAction.hh"
#include "SLArUserPhotonTrackInformation.hh"
#include "SLArDetectorConstruction.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArTrackingAction::SLArTrackingAction() {}

SLArTrackingAction::~SLArTrackingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{

  //Let this be up to the user via vis.mac
  if (aTrack->GetParticleDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
  {
    fpTrackingManager->SetStoreTrajectory(true);
  }
  else {
    fpTrackingManager->SetStoreTrajectory(false);
    //This user track information is only relevant to the photons
    fpTrackingManager->SetUserTrackInformation(
        new SLArUserPhotonTrackInformation);
  }
  //Use custom trajectory class
  fpTrackingManager->SetTrajectory(new SLArTrajectory(aTrack));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArTrackingAction::PostUserTrackingAction(const G4Track* aTrack){

  SLArTrajectory* trajectory =
    (SLArTrajectory*)fpTrackingManager->GimmeTrajectory();

  //Lets choose to draw only the photons that hit the sphere and a pmt
  if(aTrack->GetDefinition()==
      G4OpticalPhoton::OpticalPhotonDefinition()){
    SLArUserPhotonTrackInformation*
      trackInformation=(SLArUserPhotonTrackInformation*)aTrack->GetUserInformation();

    /*
     *const G4VProcess* creator=aTrack->GetCreatorProcess();
     *if(creator && creator->GetProcessName()=="OpWLS"){
     *  trajectory->WLS();
     *  trajectory->SetDrawTrajectory(true);
     *}
     */

    //if((trackInformation->GetTrackStatus()&hitPMT)|| 
       //(trackInformation->GetTrackStatus()&absorbed) ||
       //(trackInformation->GetTrackStatus()&boundaryAbsorbed) )
    trajectory->SetDrawTrajectory(true);

    if (trackInformation) {
      if(trackInformation->GetForceDrawTrajectory())
        trajectory->SetDrawTrajectory(true);
    }
  }
  else //draw all other trajectories and store them in SLArMCPrimaryInfo
  {
    trajectory->SetDrawTrajectory(true);
/*
 *    // Copy relevant attributes into SLArEvTrajectory
 *    SLArEventTrajectory* evTrajectory = new SLArEventTrajectory();
 *    evTrajectory->SetParticleName(trajectory->GetParticleName());
 *    evTrajectory->SetPDGID(trajectory->GetPDGEncoding());
 *    evTrajectory->SetTrackID(trajectory->GetTrackID());
 *    evTrajectory->SetParentID(trajectory->GetParentID());
 *    // store trajectory points
 *    for (int n=0; n<trajectory->GetPointEntries(); n++)
 *      evTrajectory->RegisterPoint(
 *          trajectory->GetPoint(n)->GetPosition().getX(),
 *          trajectory->GetPoint(n)->GetPosition().getY(),
 *          trajectory->GetPoint(n)->GetPosition().getZ()
 *          );
 *
 *    // store SLArEventTrajectory into SLArMCPrimaryInfo
 *    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
 *    SLArMCPrimaryInfo* evInfo = SLArAnaMgr->GetEvent()->GetPrimary();
 *    evInfo->RegisterTrajectory(evTrajectory);
 */
  }  


}
