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
/// \file SLAr/include/SLArPrimaryGeneratorMessenger.hh
/// \brief Definition of the SLArPrimaryGeneratorMessenger class
//
//
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef SLArPrimaryGeneratorMessenger_h
#define SLArPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class SLArPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWithAString;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SLArPrimaryGeneratorMessenger: public G4UImessenger
{
  public:
    SLArPrimaryGeneratorMessenger(SLArPrimaryGeneratorAction* );
    virtual ~SLArPrimaryGeneratorMessenger();
 
    virtual void SetNewValue(G4UIcommand*, G4String);
 
  private:
    SLArPrimaryGeneratorAction*         fSLArAction;
    G4UIdirectory*                      fCmdGunDir;
    G4UIcmdWith3VectorAndUnit*          fCmdGunPosition;
    G4UIcmdWith3Vector*                 fCmdGunDirection;

    G4UIcmdWithAString*                 fCmdDirectionMode;
    G4UIcmdWithAString*                 fCmdGunMode;
    G4UIcmdWithAString*                 fCmdBulkVol;
    G4UIcmdWithAString*                 fCmdMarley;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
