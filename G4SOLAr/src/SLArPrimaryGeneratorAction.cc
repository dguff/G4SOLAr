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

#include "Randomize.hh"

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
   fParticleGun(0)
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

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

  fGunMode    = kFixed;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorAction::~SLArPrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if (fGunMode == kCosmic) 
  {
    //*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *//
    // by default set random angle (cos2 theta) in the range where it 
    // is more likely for it to intercept the hodoscopes. 
    // The tracks are required to cross a disk of 5 cm radius in the 
    // center of the tank

    G4double h_world = 0;
    G4VPhysicalVolume* worldPV
      = G4PhysicalVolumeStore::GetInstance()->GetVolume("World");
    if (worldPV) {
      G4Box* worldSV 
        = dynamic_cast<G4Box*>(worldPV->GetLogicalVolume()->GetSolid());
      h_world= worldSV->GetZHalfLength();
    }


    G4double h_tank = 0;
    G4double y_tank = 0;
    G4VPhysicalVolume* tankPV
      = G4PhysicalVolumeStore::GetInstance()->GetVolume("Tank");
    if (tankPV) y_tank = tankPV->GetObjectTranslation().getY();
    if (tankPV) {
      G4Tubs* tankSV 
        = dynamic_cast<G4Tubs*>(tankPV->GetLogicalVolume()->GetSolid());
      h_tank = tankSV->GetZHalfLength();
    }

    // FIXME: when hodoscopes are better treated in detector construction
    // replace hard coded quota and size
    G4double y_hodo = 30*cm;  
    G4double x_hodo = 15*cm;

    // compute maximum theta allowed
    G4double theta_max = atan( x_hodo / (y_hodo-y_tank) );
    // define theta distribution in the allowed range
    TF1 fCRtheta("fCRtheta", "pow(cos(x), 2)", -theta_max, +theta_max);
    // Set particle momentum
    G4ParticleMomentum p(+0., -1., +0.  ); // initial vertical track
    p.rotateZ( fCRtheta.GetRandom()     ); // rotate according to theta
    p.rotateY( G4UniformRand()*360.0*deg); // phi - symmetry

    // define crossing (x,z) coordinates 
    G4double r2 = G4UniformRand()* 25.*cm2;
    G4double phi= G4UniformRand()*360.*deg;
    G4double  xx= sqrt(r2)*cos(phi);
    G4double  zz= sqrt(r2)*sin(phi);

    // trace back the track from (xx, zz, y_tank) to the top of
    // the world box
    G4double scale = (h_world - y_tank) / p.getY();

    G4ThreeVector pos(p.getX()*scale+xx    , 
        p.getY()*scale+y_tank, 
        p.getZ()*scale+zz    );

    // Set gun position
    fParticleGun
      ->SetParticlePosition(pos);
    // Set gun direction
    fParticleGun->SetParticleMomentumDirection(p);

    G4cout << "h_world: " << h_world << G4endl;
    G4cout << "h_tank          : " << h_tank           << G4endl;
    G4cout << "theta_max:        " << theta_max        << G4endl;
    G4cout << "vtx pos  :        " << pos              << G4endl;
    G4cout << "momentum dir:     " << p                << G4endl;
  }

  // Store Primary information id dst

  
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
  SLArMCPrimaryInfo * tc_primary = SLArAnaMgr->GetEvent()->GetPrimary();
  tc_primary->ResetParticle();

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

  fParticleGun->GeneratePrimaryVertex(anEvent);
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
