/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArLArSD.cc
 * @created     : giovedì nov 03, 2022 12:44:17 CET
 */


#include "SLArAnalysisManager.hh"
#include "SLArUserTrackInformation.hh"
#include "SLArTrackingAction.hh"
#include "SLArRunAction.hh"
#include "SLArTrajectory.hh"
#include "SLArScintillation.h"
#include "detector/TPC/SLArLArSD.hh"
#include "detector/TPC/SLArLArHit.hh"
#include "physics/SLArElectronDrift.hh"

#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4TrackingManager.hh"
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

    SLArRunAction* runAction = 
      (SLArRunAction*)G4RunManager::GetRunManager()->GetUserRunAction(); 
  

    SLArAnalysisManager* anaMngr = SLArAnalysisManager::Instance(); 

    // Get hit from collection
    SLArLArHit* hit = (*fHitsCollection)[0];

    int n_ph = 0; 
    int n_el = 0; 
    auto trackingAction = 
      (SLArTrackingAction*)G4EventManager::GetEventManager()->GetUserTrackingAction(); 
    
    auto stepMngr = trackingAction->GetTrackingManager()->GetSteppingManager(); 
    if (stepMngr->GetfStepStatus() != fAtRestDoItProc) {
      G4ProcessVector* process_vector = stepMngr->GetfPostStepDoItVector(); 
      for (size_t iproc = 0; iproc < stepMngr->GetMAXofPostStepLoops(); iproc++) {
        G4VProcess* proc = (*process_vector)[iproc]; 

        if (proc->GetProcessName() == "Scintillation") {
          SLArScintillation* scint_process = (SLArScintillation*)proc; 

          n_ph = scint_process->GetNumPhotons(); 
          n_el = scint_process->GetNumIonElectrons(); 
          
          break;
        } 
      }
    }

    runAction->GetElectronDrift()->Drift(n_el, 
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
