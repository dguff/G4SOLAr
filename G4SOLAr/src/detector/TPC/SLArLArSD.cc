/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArLArSD.cc
 * @created     : giovedì nov 03, 2022 12:44:17 CET
 */


#include "SLArAnalysisManager.hh"
#include "SLArTrackingAction.hh"
#include "SLArRunAction.hh"
#include "SLArTrajectory.hh"
#include "detector/TPC/SLArLArSD.hh"
#include "detector/TPC/SLArLArHit.hh"
#include "physics/SLArElectronDrift.hh"

#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VProcess.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4PhysicalConstants.hh"
#include "G4OpticalPhoton.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArLArSD::SLArLArSD(G4String name)
  : G4VSensitiveDetector(name), fHitsCollection(0), 
    fHCID(-4) 
{
  collectionName.insert("LArColl");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArLArSD::~SLArLArSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArLArSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection 
    = new SLArLArHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0)
  { fHCID = G4SDManager::GetSDMpointer()
    ->GetCollectionID(fHitsCollection); }

  hce->AddHitsCollection(fHCID,fHitsCollection);

  // Add single hit for the entire target volume
  fHitsCollection->insert(new SLArLArHit());
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SLArLArSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4StepPoint* preStepPoint  = step->GetPreStepPoint();
  G4StepPoint* postStepPoint = step->GetPostStepPoint();

  G4double     edep          = step->GetTotalEnergyDeposit();

  G4TouchableHistory* touchable
    = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

  if (step->GetTrack()->GetDynamicParticle()
      ->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {

    SLArTrackingAction* trackingAction = 
      (SLArTrackingAction*)G4EventManager::GetEventManager()->GetUserTrackingAction(); 

    SLArRunAction* runAction = 
      (SLArRunAction*)G4RunManager::GetRunManager()->GetUserRunAction(); 

    SLArAnalysisManager* anaMngr = SLArAnalysisManager::Instance(); 

    // Get hit from collection
    SLArLArHit* hit = (*fHitsCollection)[0];
    SLArTrajectory* trajectory =
      (SLArTrajectory*)trackingAction->GetTrackingManager()->GimmeTrajectory();
    int n_ion = trajectory->GetIonElectrons().back();

    runAction->GetElectronDrift()->Drift(n_ion, 
        step->GetTrack()->GetTrackID(), 
        0.5*(postStepPoint->GetPosition()+preStepPoint->GetPosition()),
        postStepPoint->GetGlobalTime(), 
        anaMngr->GetAnodeCfg(touchable->GetCopyNumber(0)), 
        anaMngr->GetEvent()->GetEventAnodeByTPCID(touchable->GetCopyNumber(0))); 

    
    hit->Add(edep);
  }     

  return true;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
