/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArReadoutTileSD
 * @created     : mercoledì ago 10, 2022 08:53:56 CEST
 */


#include "detector/ReadoutTile//SLArReadoutTileSD.hh"
#include "detector/ReadoutTile//SLArReadoutTileHit.hh"

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArReadoutTileSD::SLArReadoutTileSD(G4String name)
: G4VSensitiveDetector(name), fHitsCollection(0), fHCID(-2)
{
    collectionName.insert("ReadoutTileColl");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArReadoutTileSD::~SLArReadoutTileSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArReadoutTileSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection 
      = new SLArReadoutTileHitsCollection(SensitiveDetectorName,collectionName[0]);
    if (fHCID<0)
    { fHCID = G4SDManager::GetSDMpointer()
              ->GetCollectionID(fHitsCollection); }

    hce->AddHitsCollection(fHCID,fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SLArReadoutTileSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{

  auto particleDef = step->GetTrack()->GetDynamicParticle()->GetParticleDefinition(); 
  if (particleDef != G4OpticalPhoton::OpticalPhotonDefinition()) {
#ifdef SLAR_DEBUG
    printf("SLArReadoutTileSD::ProcessHits() WARNING: ");
    printf("ReadoutTile is an optical detector, while this hit comes from a %s\n", 
        particleDef->GetParticleName().c_str());
#endif
  } else {
#ifdef SLAR_DEBUG
    printf("SLArReadoutTileSD::ProcessHits() WARNING ");
    printf("OpticalPhotons should be processed by ProcessHits_constStep\n");
#endif
  }    

  return true;
}

G4bool SLArReadoutTileSD::ProcessHits_constStep(const G4Step* step,
                                       G4TouchableHistory* ){

  G4Track* track = step->GetTrack();
  if(track->GetDefinition()
     != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  G4double phEne = 0*CLHEP::eV;
  G4StepPoint* preStepPoint  = step->GetPreStepPoint();
  G4StepPoint* postStepPoint = step->GetPostStepPoint();
  
  G4TouchableHistory* touchable
    = (G4TouchableHistory*)(step->GetPostStepPoint()->GetTouchable());

  G4ThreeVector worldPos 
    = postStepPoint->GetPosition();
  G4ThreeVector localPos
    = touchable->GetHistory()
      ->GetTopTransform().TransformPoint(worldPos);
 
  SLArReadoutTileHit* hit = nullptr;
  if (track->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) 
  {
    // Get the creation process of optical photon
    G4String procName = "";
    if (track->GetTrackID() != 1) // make sure consider only secondaries
    {
      procName = track->GetCreatorProcess()->GetProcessName();
    }
    phEne = track->GetTotalEnergy();

    hit = new SLArReadoutTileHit(); //so create new hit
    hit->SetPhotonWavelength( CLHEP::h_Planck * CLHEP::c_light / phEne * 1e6);
    hit->SetWorldPos(worldPos);
    hit->SetLocalPos(localPos);
    hit->SetTime(preStepPoint->GetGlobalTime());
    hit->SetMegaTileIdx(touchable->GetCopyNumber(6));
    hit->SetRowTileIdx(touchable->GetCopyNumber(5));
    hit->SetTileIdx(touchable->GetCopyNumber(4));
    hit->SetPhotonProcess(procName);
    
    fHitsCollection->insert(hit);
  }
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
