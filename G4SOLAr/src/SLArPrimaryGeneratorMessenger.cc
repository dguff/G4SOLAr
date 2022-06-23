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
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorMessenger::
  SLArPrimaryGeneratorMessenger(SLArPrimaryGeneratorAction* SLArGun)
  : G4UImessenger(),
    fSLArAction(SLArGun)
{
  fGunDir = new G4UIdirectory("/SLAr/gun/");
  fGunDir->SetGuidance("PrimaryGenerator control");

  fPolarCmd =
    new G4UIcmdWithADoubleAndUnit("/BeamCell/gun/optPhotonPolar",this);
  fPolarCmd->SetGuidance("Set linear polarization");
  fPolarCmd->SetGuidance("  angle w.r.t. (k,n) plane");
  fPolarCmd->SetParameterName("angle",true);
  fPolarCmd->SetUnitCategory("Angle");
  fPolarCmd->SetDefaultValue(-360.0);
  fPolarCmd->SetDefaultUnit("deg");
  fPolarCmd->AvailableForStates(G4State_Idle);

  fModeCmd = 
    new G4UIcmdWithAString("/SLAr/gun/mode", this);
  fModeCmd->SetGuidance("Set SOLAr gun mode");
  fModeCmd->SetGuidance("(Fixed, Radio, ...)");
  fModeCmd->SetParameterName("Mode", false);
  fModeCmd->SetDefaultValue("Fixed");
  fModeCmd->SetCandidates("Fixed Radio");

  fVolCmd = 
    new G4UIcmdWithAString("/SLAr/gun/volume", this); 
  fVolCmd->SetGuidance("Set bulk volume for bulk event generation"); 
  fVolCmd->SetGuidance("(Physical Volume name)"); 
  fVolCmd->SetParameterName("PhysVol", true, false); 
  fVolCmd->SetDefaultValue("Target"); 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorMessenger::~SLArPrimaryGeneratorMessenger()
{
  delete fPolarCmd;
  delete fModeCmd;
  delete fGunDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorMessenger::SetNewValue(
              G4UIcommand* command, G4String newValue)
{
  if( command == fPolarCmd ) 
  {
    G4double angle = fPolarCmd->GetNewDoubleValue(newValue);
    if ( angle == -360.0*deg ) {
      fSLArAction->SetOptPhotonPolar();
    } else {
      fSLArAction->SetOptPhotonPolar(angle);
    }
  }
  else if (command == fModeCmd) 
  {
    EGunMode gunMode = kFixed;
    G4String strMode = newValue;
    if      (strMode.contains("Fixed" )) gunMode = kFixed;
    else if (strMode.contains("Radio")) gunMode = kRadio;

    fSLArAction->SetGunMode(gunMode);
  } 
  else if (command == fVolCmd) { 
    G4String vol = newValue; 
    fSLArAction->SetBulkName(vol); 
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
