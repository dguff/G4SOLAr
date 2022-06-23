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
/// \file SLAr/src/SLArPrimaryGeneratorAction.cc
/// \brief Implementation of the SLArPrimaryGeneratorAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "SLArAnalysisManager.hh"
#include "TF1.h"

#include "SLArPrimaryGeneratorAction.hh"
#include "SLArPrimaryGeneratorMessenger.hh"
#include "SLArBulkVertexGenerator.hh"
#include "bxdecay0_g4/primary_generator_action.hh"

#include "Randomize.hh"

#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorAction::SLArPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(), 
   fParticleGun(0), fDecay0Gen(0), fGunMessenger(0), 
   fBulkGenerator(0), 
   fVolumeName("Target"), 
   fGunMode(kFixed)
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);
  fDecay0Gen   = new bxdecay0_g4::PrimaryGeneratorAction(0);
  fBulkGenerator = new SLArBulkVertexGenerator(); 
  fDecay0Gen->SetVertexGenerator(fBulkGenerator);

  //create a messenger for this class
  fGunMessenger = new SLArPrimaryGeneratorMessenger(this);

  //default kinematic
  //
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("mu-");

  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleTime(0.0*ns);
  fParticleGun->SetParticleEnergy(3.*GeV);

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, +1) );
  fParticleGun->SetParticlePosition         (G4ThreeVector(0, 0, -1.5*m));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorAction::~SLArPrimaryGeneratorAction()
{
  delete fGunMessenger;
  delete fParticleGun;
  delete fDecay0Gen;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorAction::SetBulkName(G4String vol) {
  fVolumeName = vol; 
  auto volume = G4PhysicalVolumeStore::GetInstance()->GetVolume(fVolumeName); 

  fBulkGenerator->SetBulkLogicalVolume(volume->GetLogicalVolume()); 
  return;
}

void SLArPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{


  //*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *//
 
  if (fGunMode == kRadio) {
    G4cerr << "Generating radio event..." << G4endl;
    if (!fBulkGenerator->GetBulkLogicalVolume()) {
      G4cerr << "Setting bulk volume to " << fVolumeName.c_str() << G4endl;
      SetBulkName(fVolumeName);
    }
    G4cerr << "Running fDecay0Gen..." << G4endl;
    fDecay0Gen->GeneratePrimaries(anEvent); 

    // Store Primary information id dst
    G4cerr << "Storing primary info..." << G4endl;
    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();

    G4int n = anEvent->GetNumberOfPrimaryVertex(); 
    for (int i=0; i<n; i++) {
      SLArMCPrimaryInfo * tc_primary = new SLArMCPrimaryInfo();

      auto particle = anEvent->GetPrimaryVertex(i)->GetPrimary();
      tc_primary->SetID  (particle->GetParticleDefinition()->GetParticleDefinitionID());
      tc_primary->SetTrackID(particle->GetTrackID());
      tc_primary->SetName(particle->GetParticleDefinition()->GetParticleName());
      tc_primary->SetPosition(anEvent->GetPrimaryVertex(i)->GetX0(),
          anEvent->GetPrimaryVertex(i)->GetY0(), 
          anEvent->GetPrimaryVertex(i)->GetZ0());
      tc_primary->SetMomentum(
          particle->GetPx(), particle->GetPy(), particle->GetPz(), 
          particle->GetKineticEnergy());

      SLArAnaMgr->GetEvent()->GetPrimary().push_back(tc_primary); 
    }
    
    return;
  } else {
    G4ThreeVector pos(0, 0, 0);
    // Set gun position
    fParticleGun->SetParticlePosition(pos);
    // Set gun direction
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
    // Store Primary information id dst
    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
    SLArMCPrimaryInfo * tc_primary = new SLArMCPrimaryInfo(); 

    tc_primary->SetID( fParticleGun->GetParticleDefinition()
        ->GetParticleDefinitionID());
    tc_primary->SetName(fParticleGun->GetParticleDefinition()
        ->GetParticleName());
    tc_primary->SetPosition(fParticleGun->GetParticlePosition().getX(),
        fParticleGun->GetParticlePosition().getY(),
        fParticleGun->GetParticlePosition().getZ());
    tc_primary->SetMomentum(fParticleGun->GetParticleMomentumDirection().getX(), 
        fParticleGun->GetParticleMomentumDirection().getY(), 
        fParticleGun->GetParticleMomentumDirection().getZ(), 
        fParticleGun->GetParticleEnergy());
    SLArAnaMgr->GetEvent()->GetPrimary().push_back(tc_primary); 

    fParticleGun->GeneratePrimaryVertex(anEvent);
  }


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorAction::SetOptPhotonPolar()
{
 G4double angle = G4UniformRand() * 360.0*deg;
 SetOptPhotonPolar(angle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorAction::SetGunMode(EGunMode gunMode)
{
  fGunMode = gunMode;
}

void SLArPrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
 if (fParticleGun->GetParticleDefinition()->GetParticleName()!="opticalphoton")
   {
     G4cout << "--> warning from PrimaryGeneratorAction::SetOptPhotonPolar() :"
               "the particleGun is not an opticalphoton" << G4endl;
     return;
   }

 G4ThreeVector normal (1., 0., 0.);
 G4ThreeVector kphoton = fParticleGun->GetParticleMomentumDirection();
 G4ThreeVector product = normal.cross(kphoton);
 G4double modul2       = product*product;
 
 G4ThreeVector e_perpend (0., 0., 1.);
 if (modul2 > 0.) e_perpend = (1./std::sqrt(modul2))*product;
 G4ThreeVector e_paralle    = e_perpend.cross(kphoton);
 
 G4ThreeVector polar = std::cos(angle)*e_paralle + std::sin(angle)*e_perpend;
 fParticleGun->SetParticlePolarization(polar);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
