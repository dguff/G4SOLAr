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
#include "SLArMarleyGen.hh"
#include "bxdecay0_g4/primary_generator_action.hh"

#include "Randomize.hh"

#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorAction::SLArPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(), 
   fParticleGun(0), fDecay0Gen(0), fMarleyGen(0), fGunMessenger(0), 
   fBulkGenerator(0), 
   fVolumeName("Target"), 
   fGunMode(kGun), 
   fGunPosition(0, 0, 0),
   fGunDirection(0, 0, 1) 
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);
  if (!fDecay0Gen) {
    fDecay0Gen = new bxdecay0_g4::PrimaryGeneratorAction(); 
    fBulkGenerator = new SLArBulkVertexGenerator(); 
    fDecay0Gen->SetVertexGenerator(fBulkGenerator); 
  }

  //create a messenger for this class
  fGunMessenger = new SLArPrimaryGeneratorMessenger(this);

  //default kinematic
  //
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("mu-");

  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleTime(0.0*CLHEP::ns);
  fParticleGun->SetParticleEnergy(3.*CLHEP::GeV);

  fParticleGun->SetParticleMomentumDirection( fGunDirection );
  fParticleGun->SetParticlePosition         ( fGunPosition  );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorAction::~SLArPrimaryGeneratorAction()
{
  delete fGunMessenger;
  delete fParticleGun;
  delete fDecay0Gen;
  delete fMarleyGen;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorAction::SetBulkName(G4String vol) {
  fVolumeName = vol; 
  auto volume = G4PhysicalVolumeStore::GetInstance()->GetVolume(fVolumeName); 

  fBulkGenerator->SetBulkLogicalVolume(volume->GetLogicalVolume()); 
  fBulkGenerator->SetSolidTranslation(volume->GetTranslation()); 
  fBulkGenerator->SetSolidRotation(volume->GetRotation()); 
  return;
}

void SLArPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

  // Store Primary information id dst
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();

  if (fDirectionMode == kRandom) {
    fGunDirection = SampleRandomDirection();
  }
  //*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *//
 
  if (fGunMode == kRadio) {
    if (!fBulkGenerator->GetBulkLogicalVolume()) {
      G4cerr << "Setting bulk volume to " << fVolumeName.c_str() << G4endl;
      SetBulkName(fVolumeName);
    }
    fDecay0Gen->GeneratePrimaries(anEvent); 
  } else if (fGunMode == kMarley) {
    if (!fMarleyGen) {
      fMarleyGen = new SLArMarleyGen(fMarleyCfg.c_str()); 
      if (!fBulkGenerator) {
        fBulkGenerator = new SLArBulkVertexGenerator();
      }
      fMarleyGen->SetVertexGenerator(fBulkGenerator); 
    }
    if (!fBulkGenerator->GetBulkLogicalVolume()) {
      G4cout << "Setting bulk volume to " << fVolumeName.c_str() << G4endl;
      SetBulkName(fVolumeName); 
    }
    fMarleyGen->SetNuDirection(fGunDirection);
    SLArAnaMgr->GetEvent()->SetDirection(fGunDirection.x(), fGunDirection.y(), fGunDirection.z()); 
    fMarleyGen->GeneratePrimaries(anEvent); 
  }
  else {
    // Set gun position
    fParticleGun->SetParticlePosition( fGunPosition );
    // Set gun direction
    fParticleGun->SetParticleMomentumDirection( fGunDirection );
    // Generate primary vertex
    fParticleGun->GeneratePrimaryVertex(anEvent);
    // Store Primary information id dst
  }

  G4int n = anEvent->GetNumberOfPrimaryVertex(); 

  //printf("Primary Generator Action produced %i vertex(ices)\n", n); 
  for (int i=0; i<n; i++) {
    SLArMCPrimaryInfo tc_primary;

    G4int np = anEvent->GetPrimaryVertex(i)->GetNumberOfParticle(); 
    //printf("vertex %i has %i particles \n", n, np); 
    for (int ip = 0; ip<np; ip++) {
      //printf("getting particle %i...\n", ip); 
      auto particle = anEvent->GetPrimaryVertex(i)->GetPrimary(ip); 

      if (!particle->GetParticleDefinition()) {
        tc_primary.SetID  (particle->GetPDGcode()); 
        tc_primary.SetName("Ion");
      } else {
        tc_primary.SetID  (particle->GetParticleDefinition()->GetParticleDefinitionID());
        tc_primary.SetName(particle->GetParticleDefinition()->GetParticleName());

      }
      tc_primary.SetTrackID(particle->GetTrackID());
      tc_primary.SetPosition(anEvent->GetPrimaryVertex(i)->GetX0(),
          anEvent->GetPrimaryVertex(i)->GetY0(), 
          anEvent->GetPrimaryVertex(i)->GetZ0());
      tc_primary.SetMomentum(
          particle->GetPx(), particle->GetPy(), particle->GetPz(), 
          particle->GetKineticEnergy());

      //printf("Adding particle to primary output list\n"); 
      //tc_primary.PrintParticle(); 
      SLArAnaMgr->GetEvent()->RegisterPrimary(new SLArMCPrimaryInfo(tc_primary)); 
    }
  }

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorAction::SetGunMode(EGunMode gunMode)
{
  fGunMode = gunMode;
}

void SLArPrimaryGeneratorAction::SetMarleyConf(G4String marley_conf) {
  fMarleyCfg = marley_conf; 
  if (!fMarleyGen) {
    fMarleyGen = new SLArMarleyGen(marley_conf); 
    if (!fBulkGenerator) {
      fBulkGenerator = new SLArBulkVertexGenerator();
      SetBulkName(fVolumeName); 
    }

  } else {
    delete fMarleyGen; 
    fMarleyGen = new SLArMarleyGen(fMarleyCfg); 
    if (!fBulkGenerator) {
      fBulkGenerator = new SLArBulkVertexGenerator(); 
      SetBulkName(fVolumeName); 
    }
  }

  fMarleyGen->SetVertexGenerator(fBulkGenerator); 
  return; 
}

G4ThreeVector SLArPrimaryGeneratorAction::SampleRandomDirection() {
  double cosTheta = 2*G4UniformRand() - 1.;
  double phi = TMath::TwoPi()*G4UniformRand();
  double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
  double ux = sinTheta*std::cos(phi),
         uy = sinTheta*std::sin(phi),
         uz = cosTheta;

  G4ThreeVector dir(ux, uy, uz);
  
  return dir; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
