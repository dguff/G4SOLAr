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
/// \file SLArSteppingAction.cc
/// \brief Implementation of the SLArSteppingAction class

#include "SLArSteppingAction.hh"
#include "SLArUserPhotonTrackInformation.hh"
#include "SLArTrajectory.hh"

#include "detector/SuperCell/SLArSuperCellSD.hh"
#include "detector/ReadoutTile/SLArReadoutTileSD.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4PVReplica.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArSteppingAction::SLArSteppingAction(SLArEventAction* ea, SLArTrackingAction* ta)
  : G4UserSteppingAction()
{ 
  fEventAction          = ea;
  fTrackinAction        = ta;
  fEventNumber = -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArSteppingAction::~SLArSteppingAction()
{ ; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();

  const G4ParticleDefinition* particleDef = track->GetDynamicParticle()->
    GetParticleDefinition();

  G4StepPoint* thePrePoint = step->GetPreStepPoint();
  G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();

  G4StepPoint* thePostPoint = step->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();
  
  if (track->GetParticleDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
    SLArTrajectory* trajectory =
      (SLArTrajectory*)fTrackinAction->GetTrackingManager()->GimmeTrajectory();
    double edep = step->GetTotalEnergyDeposit();
    trajectory->AddEdep(edep);
    //printf("trk ID %i [%i], PDG ID %i [%i] - edep size %lu - trj size %i\n", 
        //track->GetTrackID(), 
        //trajectory->GetTrackID(), 
        //track->GetParticleDefinition()->GetPDGEncoding(),
        //trajectory->GetPDGEncoding(), 
        //trajectory->GetEdep().size(), 
        //trajectory->GetPointEntries());
  }

  if (!thePostPV) return;

  G4OpBoundaryProcessStatus boundaryStatus=Undefined;
  static G4ThreadLocal G4OpBoundaryProcess* boundary = nullptr;

  if (particleDef == G4OpticalPhoton::OpticalPhotonDefinition() && 
      thePrePV != thePostPV) {

    SLArUserPhotonTrackInformation* phInfo = 
      (SLArUserPhotonTrackInformation*)track->GetUserInformation();

    //find the boundary process only once
    if(!boundary){
      G4ProcessManager* pm
        = track->GetDefinition()->GetProcessManager();
      G4int nprocesses = pm->GetProcessListLength();
      G4ProcessVector* pv = pm->GetProcessList();
      G4int i;
      for( i=0;i<nprocesses;i++){
        if((*pv)[i]->GetProcessName()=="OpBoundary"){
          boundary = (G4OpBoundaryProcess*)(*pv)[i];
//#ifdef SLAR_DEBUG
          //G4cout<< "Optical ph at boundary!" << G4endl; 
//#endif
          break;
        }
      }
    }


    //Was the photon absorbed by the absorption process
    // [from LXe example]
    if(thePostPoint->GetProcessDefinedStep()->GetProcessName()
       =="OpAbsorption"){
      fEventAction->IncAbsorption();
      phInfo->AddTrackStatusFlag(absorbed);
    }

    boundaryStatus=boundary->GetStatus();
    //Check to see if the partcile was actually at a boundary
    //Otherwise the boundary status may not be valid
    //Prior to Geant4.6.0-p1 this would not have been enough to check
    if(thePostPoint->GetStepStatus()==fGeomBoundary){
      if(fExpectedNextStatus==StepTooSmall){
        if(boundaryStatus!=StepTooSmall){
          G4ExceptionDescription ed;
          ed << "SLArSteppingAction::UserSteppingAction(): "
            << "No reallocation step after reflection!"
            << G4endl;
          G4Exception("SLArSteppingAction::UserSteppingAction()", 
              "LXeExpl01",
              FatalException,ed,
              "Something is wrong with the surface normal or geometry");
        }
      }
      fExpectedNextStatus=Undefined;
      switch(boundaryStatus){
        case Absorption:
          {
//#ifdef SLAR_DEBUG
            //G4cout << "SLArSteppingAction::UserSteppingAction Absorption" << G4endl;
//#endif
            phInfo->AddTrackStatusFlag(boundaryAbsorbed);
            fEventAction->IncBoundaryAbsorption();
            break;
          }
        case NoRINDEX:
//#ifdef SLAR_DEBUG
          //printf("SLArSteppingAction::UserSteppingAction NoRINDEX\n");
          //printf("ph E = %.2f eV; pre/post step point volume: %s/%s\n", 
              //track->GetTotalEnergy()*1e6,
              //thePrePV->GetName().c_str(), thePostPV->GetName().c_str()); 
//#endif
          break;
        case Detection: 
          //Note, this assumes that the volume causing detection
          //is the photocathode because it is the only one with
          //non-zero efficiency
          {
            //Triger sensitive detector manually since photon is
            //absorbed but status was Detection
            G4TouchableHistory* touchable = 
              (G4TouchableHistory*)thePostPoint->GetTouchable(); 
#ifdef SLAR_DEBUG
            G4cout << "SLArSteppingAction::UserSteppingAction Detection" << G4endl;
#endif
            G4SDManager* SDman = G4SDManager::GetSDMpointer();
            G4String volName = touchable->GetVolume()->GetName();

            G4String sdNameSiPM  ="/tile/sipm";
            G4String sdNameSC    ="/supercell";

            SLArReadoutTileSD* sipmSD = nullptr;
            SLArSuperCellSD* supercellSD = nullptr; 

#ifdef SLAR_DEBUG
             printf("Detection in %s - copy id [%i]\n", 
                 volName.c_str(), touchable->GetCopyNumber(0)); 
#endif

            phInfo->AddTrackStatusFlag(hitPMT);
            if (volName=="sipm") {
#ifdef SLAR_DEBUG
              printf("Copy No hierarchy: [%i, %i, %i, %i, %i, %i, %i]\n", 
                  touchable->GetCopyNumber(0), 
                  touchable->GetCopyNumber(1),
                  touchable->GetCopyNumber(2),
                  touchable->GetCopyNumber(3),
                  touchable->GetCopyNumber(4),
                  touchable->GetCopyNumber(5), 
                  touchable->GetCopyNumber(6) 
                  );
#endif
              sipmSD = (SLArReadoutTileSD*)SDman->FindSensitiveDetector(sdNameSiPM);
              if(sipmSD) { 
                sipmSD->ProcessHits_constStep(step, nullptr);
              } else {
#ifdef SLAR_DEBUG
                printf("SLArSteppingAction::UserSteppingAction::Detection WARNING\n"); 
                printf("%s is not recognized as SD\n", volName.c_str());
#endif
              }
            } else if (volName == "SuperCellCoating") {
#ifdef SLAR_DEBUG
              printf("Copy No hierarchy: [%i, %i, %i]\n", 
                  touchable->GetCopyNumber(0), 
                  touchable->GetCopyNumber(1),
                  touchable->GetCopyNumber(2)
                  );
#endif

              supercellSD = (SLArSuperCellSD*)SDman->FindSensitiveDetector(sdNameSC);
              if(supercellSD) { 
                supercellSD->ProcessHits_constStep(step, nullptr);
              } else {
#ifdef SLAR_DEBUG
                printf("SLArSteppingAction::UserSteppingAction::Detection WARNING\n"); 
                printf("%s is not recognized as SD\n", volName.c_str());
#endif
              }
            }
            
            track->SetTrackStatus( fStopAndKill );
            break;
          }
        default:
          break;
      }

    }
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
