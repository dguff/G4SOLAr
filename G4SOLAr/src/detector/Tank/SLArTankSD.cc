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
// $Id: SLArTankSD.cc 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file SLArTankSD.cc
/// \brief Implementation of the SLArTankSD class

#include "SLArAnalysisManager.hh"
#include "detector/Tank/SLArTankSD.hh"
#include "detector/Tank/SLArTankHit.hh"
#include "SLArAnalysis.hh"

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
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArTankSD::SLArTankSD(G4String name)
  : G4VSensitiveDetector(name), fHitsCollection(0), 
    fHCID(-4) 
{
  collectionName.insert("TargetColl");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArTankSD::~SLArTankSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArTankSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection 
    = new SLArTankHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0)
  { fHCID = G4SDManager::GetSDMpointer()
    ->GetCollectionID(fHitsCollection); }

  hce->AddHitsCollection(fHCID,fHitsCollection);

  // Add single hit for the entire target volume
  fHitsCollection->insert(new SLArTankHit());
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SLArTankSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  //G4StepPoint* preStepPoint  = step->GetPreStepPoint();
  G4StepPoint* postStepPoint = step->GetPostStepPoint();

  G4double     edep          = step->GetTotalEnergyDeposit();

  G4TouchableHistory* touchable
    = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

  if (step->GetTrack()->GetDynamicParticle()
      ->GetDefinition()->GetParticleName() != "opticalphoton") {
    G4int moduleID = touchable->GetCopyNumber(1);
    G4int    barID = touchable->GetCopyNumber(0);

    // Get hit from collection
    SLArTankHit* hit = (*fHitsCollection)[0];
    //G4cerr << "SLArTankSD touchableCopyNo1 id = "  << moduleID 
           //<< "SLArTankSD touchableCopyNo0 id = "  << barID << G4endl;

    hit->Add(edep);

    // add to edep tomography 
    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
    SLArAnaMgr->GetEvent()->GetPrimary()->AddEdep(
        postStepPoint->GetPosition()[0], 
        postStepPoint->GetPosition()[2], 
        postStepPoint->GetPosition()[1], 
        edep
        );
  }     

  return true;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
