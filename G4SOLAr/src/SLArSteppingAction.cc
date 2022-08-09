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

#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
//#include "detector/LAPPD/SLArLAPPDSD.hh"
//#include "detector/PMT/SLArPMTSD.hh"
//#include "detector/Hodoscope/SLArHodoscopeSD.hh"

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

  G4String ParticleName = track->GetDynamicParticle()->
    GetParticleDefinition()->GetParticleName();

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

  const std::vector<const G4Track*>* secondaries =
    step->GetSecondaryInCurrentStep();


/*
 *  if (secondaries->size()>0) {
 *    for(unsigned int i=0; i<secondaries->size(); ++i) {
 *      if (secondaries->at(i)->GetParentID()>0) {
 *          
 *        if(secondaries->at(i)->GetDynamicParticle()->
 *            GetParticleDefinition()
 *            == G4OpticalPhoton::OpticalPhotonDefinition())
 *        {
 *          SLArUserPhotonTrackInformation* phInfo = 
 *            (SLArUserPhotonTrackInformation*)track->GetUserInformation();
 *
 *          if 
 *            (secondaries->at(i)->GetCreatorProcess()->GetProcessName()
 *              == "Scintillation")
 *              fEventAction->IncPhotonCount_Scnt();
 *          else if 
 *            (secondaries->at(i)->GetCreatorProcess()->GetProcessName()
 *              == "Cerenkov")
 *              fEventAction->IncPhotonCount_Cher();
 *          else if 
 *            (secondaries->at(i)->GetCreatorProcess()->GetProcessName()
 *              == "WLS")
 *             fEventAction->IncPhotonCount_WLS(); 
 *        }
 *      }
 *    }
 *  }
 */


  if (!thePostPV) return;

  G4OpBoundaryProcessStatus boundaryStatus=Undefined;
  static G4ThreadLocal G4OpBoundaryProcess* boundary = nullptr;

  if (ParticleName == "opticalphoton" && 
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
            //G4cout << "SLArSteppingAction Absorption" << G4endl;
            phInfo->AddTrackStatusFlag(boundaryAbsorbed);
            fEventAction->IncBoundaryAbsorption();
            break;
          }
        case Detection: 
          //Note, this assumes that the volume causing detection
          //is the photocathode because it is the only one with
          //non-zero efficiency
          {
            //Triger sensitive detector manually since photon is
            //absorbed but status was Detection
            //G4cout << "SLArSteppingAction Detection" << G4endl;
            phInfo->AddTrackStatusFlag(hitPMT);
            //G4SDManager* SDman = G4SDManager::GetSDMpointer();
            //G4String volName = 
              //step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
            //G4String sdNameLAPPD="/LAPPD/Cathode";
            //G4String sdNamePMT  ="/PMT/Cathode";
            
            // Set LAPPD SD null unless the photon hit the LAPPD Cathode
            // (proving that LAPPD exists)
            //SLArLAPPDSD* lappdSD  = 0;
            //SLArPMTSD* pmtSD = 
              //(SLArPMTSD*)SDman->FindSensitiveDetector(sdNamePMT);
            //G4cout << "Detection in " << volName << G4endl;
            //if (volName=="PMTCathode")
            //{
              //if(pmtSD) 
              //{ 
                //pmtSD  ->ProcessHits_constStep(step, nullptr);
                ////G4cout << "PMT hit processed" << G4endl;
              //}
            //}
            //else if (volName=="LAPPDCathode")
            //{
              //lappdSD = (SLArLAPPDSD*)SDman->FindSensitiveDetector(sdNameLAPPD);
              //if(lappdSD)
              //{
                //lappdSD->ProcessHits_constStep(step, nullptr);
                ////G4cout << "LAPPD hit processed" << G4endl;
              //}
            //}
            //track->SetTrackStatus( fStopAndKill );
            break;
          }
        default:
          break;
      }

    }
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
