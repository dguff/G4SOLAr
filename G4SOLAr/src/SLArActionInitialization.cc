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
//
/// \file SLArActionInitialization.cc
/// \brief Implementation of the SLArActionInitialization class

#include "SLArActionInitialization.hh"
#include "SLArPrimaryGeneratorAction.hh"
#include "SLArRunAction.hh"
#include "SLArEventAction.hh"
#include "SLArSteppingAction.hh"
#include "SLArStackingAction.hh"
#include "SLArTrackingAction.hh"
#include "SLArSteppingVerbose.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArActionInitialization::SLArActionInitialization()
 : G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArActionInitialization::~SLArActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArActionInitialization::BuildForMaster() const
{
  //SetUserAction(new SLArRunAction());
  SetUserAction(new SLArEventAction());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArActionInitialization::Build() const
{
  SLArEventAction* eventAction = new SLArEventAction();
  SetUserAction(eventAction);


  SetUserAction(new SLArPrimaryGeneratorAction());
  SetUserAction(new SLArRunAction());
  SetUserAction(new SLArSteppingAction(eventAction));
  SetUserAction(new SLArStackingAction(eventAction));
  SetUserAction(new SLArTrackingAction());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSteppingVerbose*
               SLArActionInitialization::InitializeSteppingVerbose() const
{
  return new SLArSteppingVerbose();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
