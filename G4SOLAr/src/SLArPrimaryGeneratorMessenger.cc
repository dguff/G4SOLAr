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
/// \file SLAr/src/SLArPrimaryGeneratorMessenger.cc
/// \brief Implementation of the SLArPrimaryGeneratorMessenger class
//
//
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SLArPrimaryGeneratorMessenger.hh"

#include "SLArPrimaryGeneratorAction.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorMessenger::
  SLArPrimaryGeneratorMessenger(SLArPrimaryGeneratorAction* SLArGun)
  : G4UImessenger(),
    fSLArAction(SLArGun)
{
  fCmdGunDir = new G4UIdirectory("/SLAr/gun/");
  fCmdGunDir->SetGuidance("PrimaryGenerator control");

  fCmdGunMode= 
   new G4UIcmdWithAString("/SLAr/gun/mode", this);
  fCmdGunMode->SetGuidance("Set SOLAr gun mode");
  fCmdGunMode->SetGuidance("(Fixed, Radio, ...)");
  fCmdGunMode->SetParameterName("Mode", false);
  fCmdGunMode->SetDefaultValue("Fixed");
  fCmdGunMode->SetCandidates("Fixed Radio Marley");

  fCmdBulkVol= 
    new G4UIcmdWithAString("/SLAr/gun/volume", this); 
  fCmdBulkVol->SetGuidance("Set bulk volume for bulk event generation"); 
  fCmdBulkVol->SetGuidance("(Physical Volume name)"); 
  fCmdBulkVol->SetParameterName("PhysVol", true, false); 
  fCmdBulkVol->SetDefaultValue("Target"); 

  fCmdMarley= 
    new G4UIcmdWithAString("/SLAr/gun/marleyconf", this); 
  fCmdMarley->SetGuidance("Set MARLEY configuration file"); 
  fCmdMarley->SetGuidance("(configuration file path)"); 
  fCmdMarley->SetParameterName("marley_config", true, false); 
  fCmdMarley->SetDefaultValue("marley_default.json"); 

  fCmdDirectionMode = 
    new G4UIcmdWithAString("/SLAr/gun/SetDirectionMode", this);
  fCmdDirectionMode->SetGuidance("Set direction mode (fixed, isotropic)");
  fCmdDirectionMode->SetParameterName("DirectionMode", true);
  fCmdDirectionMode->SetDefaultValue("fixed");


  fCmdGunPosition = 
    new G4UIcmdWith3VectorAndUnit("/SLAr/gun/position", this);
  fCmdGunPosition->SetGuidance("Set position of the generated events");
  fCmdGunPosition->SetParameterName("posX", "posY", "posZ", false);
  fCmdGunPosition->SetDefaultValue(G4ThreeVector(0*CLHEP::cm, 0*CLHEP::cm, 0*CLHEP::cm));

  fCmdGunDirection = 
    new G4UIcmdWith3Vector("/SLAr/gun/direction", this);
  fCmdGunDirection->SetGuidance("Set event momentum direction");
  fCmdGunDirection->SetParameterName("p_x", "p_y", "p_z", false); 
  fCmdGunDirection->SetDefaultValue( G4ThreeVector(0, 0, 1)); 

  fCmdTracePhotons = 
    new G4UIcmdWithABool("/SLAr/DoTracePhotons", this); 
  fCmdTracePhotons->SetGuidance("Set/unset tracing of optical photons"); 
  fCmdTracePhotons->SetParameterName("do_trace", false, true); 
  fCmdTracePhotons->SetDefaultValue(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorMessenger::~SLArPrimaryGeneratorMessenger()
{
  delete fCmdGunMode;
  delete fCmdBulkVol;
  delete fCmdMarley;
  delete fCmdGunPosition;
  delete fCmdGunDirection;
  delete fCmdGunDir;
  delete fCmdTracePhotons;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorMessenger::SetNewValue(
              G4UIcommand* command, G4String newValue)
{

  if (command == fCmdGunMode) 
  {
    EGunMode gunMode = kGun;
    G4String strMode = newValue;
    if      (G4StrUtil::contains(strMode, "Gun"   )) gunMode = kGun;
    else if (G4StrUtil::contains(strMode, "Radio" )) gunMode = kRadio;
    else if (G4StrUtil::contains(strMode, "Marley")) gunMode = kMarley;

    fSLArAction->SetGunMode(gunMode);
  } 
  else if (command == fCmdBulkVol) { 
    G4String vol = newValue; 
    fSLArAction->SetBulkName(vol); 
  }
  else if (command == fCmdMarley) {
    fSLArAction->SetMarleyConf(newValue); 
  } 
  else if (command == fCmdDirectionMode) {
    if (G4StrUtil::contains(newValue, "fixed")) {
      fSLArAction->SetDirectionMode(kFixed); 
    } else if (
        G4StrUtil::contains(newValue, "random") || 
        G4StrUtil::contains(newValue, "isotropic")) 
    {
      fSLArAction->SetDirectionMode(kRandom);
    } else {
      G4cout << "WARNING: unknown key " << newValue
        << ". I will assume you want it isotropic" << G4endl; 
      fSLArAction->SetDirectionMode(kRandom);
    }
  }
  else if (command == fCmdGunPosition) {
    G4ThreeVector pos = fCmdGunPosition->GetNew3VectorValue(newValue); 
    fSLArAction->SetGunPosition( pos ); 
  }
  else if (command == fCmdGunDirection) {
    G4ThreeVector dir = fCmdGunDirection->GetNew3VectorValue(newValue); 
    fSLArAction->SetGunDirection(dir); 
  }
  else if (command == fCmdTracePhotons) {
    bool do_trace = fCmdTracePhotons->GetNewBoolValue(newValue); 
    fSLArAction->SetTraceOptPhotons(do_trace); 
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
