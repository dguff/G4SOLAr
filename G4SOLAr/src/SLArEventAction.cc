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
// $Id: SLArEventAction.cc 94486 2015-11-19 08:33:37Z gcosmo $
//
/// \file SLArEventAction.cc
/// \brief Implementation of the SLArEventAction class

#include "SLArAnalysisManager.hh"
#include "SLArEventAction.hh"
#include "SLArTrajectory.hh"
//#include "detector/LAPPD/SLArLAPPDHit.hh"
#include "detector/Tank/SLArTankHit.hh"
//#include "detector/PMT/SLArPMTHit.hh"
//#include "detector/Hodoscope/SLArHodoscopeHit.hh"
#include "SLArAnalysis.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"

#include "G4ios.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArEventAction::SLArEventAction()
: G4UserEventAction(), 
  fLAPPDHCollID(-1),
  fPMTHCollID  (-2), 
  fHodoHCollID (-3), 
  fTargetHCollID(-4)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  fHitCount                = 0;
  fPhotonCount_Scnt        = 0;
  fPhotonCount_Cher        = 0;
  fPhotonCount_WLS         = 0;
  fAbsorptionCount         = 0;
  fBoundaryAbsorptionCount = 0;
  fTotEdep                 = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArEventAction::~SLArEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArEventAction::BeginOfEventAction(const G4Event*)
{

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    //if (fLAPPDHCollID==-1) 
      //fLAPPDHCollID= sdManager->GetCollectionID("LAPPDColl");
    //if (fPMTHCollID == -2) 
      //fPMTHCollID  = sdManager->GetCollectionID("PMTColl"  );
    if (fTargetHCollID == -4)
      fTargetHCollID = sdManager->GetCollectionID("TargetColl");

    //G4cout << "SLArEventAction::BeginOfEventAction(): ";
    //G4cout << "LAPPD  ID = " << fLAPPDHCollID  << G4endl;
    //G4cout << "PMT    ID = " << fPMTHCollID    << G4endl;
    //G4cout << "Hod    ID = " << fHodoHCollID   << G4endl;
    //G4cout << "Target ID = " << fTargetHCollID << G4endl;

    // Reset hits in DST event 
    //SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
    //SLArAnaMgr->GetEvent()->GetPMTSystem()->ResetHits();

    // reset counters
    fHitCount                = 0;
    fPhotonCount_Scnt        = 0;
    fPhotonCount_Cher        = 0;
    fPhotonCount_WLS         = 0;
    fAbsorptionCount         = 0;
    fBoundaryAbsorptionCount = 0;
    fTotEdep                 = 0;

}     

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArEventAction::EndOfEventAction(const G4Event* event)
{
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (!hce) 
    {
        G4ExceptionDescription msg;
        msg << "No hits collection of this event found." << G4endl; 
        G4Exception("SLArEventAction::EndOfEventAction()",
                    "SLArCode001", JustWarning, msg);
        return;
    }   

    RecordEventTarget( event );

    //RecordEventLAPPD ( event );

    //RecordEventPMT   ( event );
  
    //RecordEventHodo ( event );

    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
    
    SLArAnaMgr->GetEvent()->SetEvNumber(event->GetEventID());
    
    SLArAnaMgr->FillEvTree();
}

//void SLArEventAction::RecordEventPMT(const G4Event* ev)
//{

  //G4HCofThisEvent* hce = ev->GetHCofThisEvent();

  //if (fPMTHCollID!= -2) 
  //{
    //// Get hits collections 
    //SLArPMTHitsCollection* hHC1 
      //= static_cast<SLArPMTHitsCollection*>(hce->GetHC(fPMTHCollID));

    //if ( (!hHC1) ) 
    //{
      //G4ExceptionDescription msg;
      //msg << "Some of hits collections of this event not found." 
        //<< G4endl; 
      //G4Exception("SLArEventAction::RecordEventPMT",
          //"SLArCode001", JustWarning, msg);
      //return;
    //}   

    ////
    //// Fill histograms & ntuple
    //// 

    //// Get analysis manager
    //G4AnalysisManager* analysisManager = 
      //G4AnalysisManager::Instance();

    //SLArAnalysisManager* SLArAnaMgr = 
      //SLArAnalysisManager::Instance();

    //// Fill histograms

    //G4int n_hit = hHC1->entries();

    //for (G4int i=0;i<n_hit;i++)
    //{
      //SLArPMTHit* hit = (*hHC1)[i];
      //if (!hit) { 
        //G4cout << "SLArEventAction::RecordEventPMT(): "
          //<< "No hits on PMTs or "
          //<< "issue in hit collection readout";
        //break;
      //}

      //G4ThreeVector localPos = hit->GetLocalPos();
      //G4ThreeVector worldPos = hit->GetWorldPos();
      //G4double      time     = hit->GetTime();
      /*
       *G4cout << "SLArEventAction::RecordEventPMT()" << G4endl;
       *G4cout << "pmt idx = " << hit->GetPMTIdx() << G4endl;
       *G4cout << "x    = " << G4BestUnit(worldPos.x(), "Length") << "; "
       *       << "y    = " << G4BestUnit(worldPos.y(), "Length") << "; "
       *       << "time = " << G4BestUnit(time, "Time") << G4endl;
       */
      //analysisManager->FillH2(1, 
                              //worldPos.x() / cm, 
                              //worldPos.z() / cm);
      
    //// Fill ntuple
    //// ntuple id = 0: PMT hits
      //analysisManager->FillNtupleIColumn(0, 0,
          //G4EventManager::GetEventManager()->GetConstCurrentEvent()
          //->GetEventID());
      //analysisManager->FillNtupleIColumn(0, 1,hit->GetPhotonProcessId());
      //analysisManager->FillNtupleIColumn(0, 2,hit->GetPMTIdx());
      //analysisManager->FillNtupleDColumn(0, 3,hit->GetTime()  );
      //analysisManager->FillNtupleDColumn(0, 4,hit->GetWorldPos().x());
      //analysisManager->FillNtupleDColumn(0, 5,hit->GetWorldPos().z());
      //analysisManager->FillNtupleDColumn(0, 6,hit->GetPhotonEnergy());
      //analysisManager->AddNtupleRow(0);

      //SLArEventHitPMT* dstHit     = new SLArEventHitPMT(
          //hit->GetTime(), 
          //hit->GetPhotonProcessId(), 
          //1.0);
      //dstHit->SetLocalPos(localPos.x(), localPos.y(), localPos.z());

      //SLArAnaMgr->GetEvent()->GetPMTSystem()
              //->RegisterHit(hit->GetPMTIdx(), 
                            //(SLArEventHitPMT*)dstHit->Clone("evNew"));
      
      //delete dstHit;
    //}

    //// Sort hits on PMTs
    //SLArAnaMgr->GetEvent()->GetPMTSystem()->SortHits();
    

    //// Print diagnostics
    //G4int printModulo = 
      //G4RunManager::GetRunManager()->GetPrintProgress();
    //if ( printModulo==0 || ev->GetEventID() % printModulo != 0) return;

    ////G4PrimaryParticle* primary = ev->GetPrimaryVertex(0)->GetPrimary(0);
    ////G4cout << G4endl
      ////<< ">>> Event " << ev->GetEventID() << " >>> Simulation truth : "
      ////<< primary->GetG4code()->GetParticleName()
      ////<< " " << primary->GetMomentum() << G4endl;

    //// Photodetector
    ////G4cout << "PMTs had " << n_hit << " hits." << G4endl;
    ////for (G4int i=0;i<n_hit;i++)
    ////{
      ////SLArPMTHit * hit = (*hHC1)[i];
      ////hit->Print();
    ////}
  //}

//}


//void SLArEventAction::RecordEventLAPPD(const G4Event* ev)
//{

  //G4HCofThisEvent* hce = ev->GetHCofThisEvent();

  //if (fLAPPDHCollID!= -1) 
  //{
    //// Get hits collections 
    //SLArLAPPDHitsCollection* hHC1 
      //= static_cast<SLArLAPPDHitsCollection*>(hce->GetHC(fLAPPDHCollID));

    //if ( (!hHC1) ) 
    //{
      //G4ExceptionDescription msg;
      //msg << "Some of hits collections of this event not found." 
        //<< G4endl; 
      //G4Exception("SLArEventAction::RecordEventLAPPD",
          //"SLArCode001", JustWarning, msg);
      //return;
    //}   

    ////
    //// Fill histograms & ntuple
    //// 

    //// Get analysis manager
    //G4AnalysisManager* analysisManager = 
      //G4AnalysisManager::Instance();

    //// Fill histograms

    //G4int n_hit = hHC1->entries();

    //for (G4int i=0;i<n_hit;i++)
    //{
      //SLArLAPPDHit* hit = (*hHC1)[i];
      //if (!hit) { 
        //G4cout << "SLArEventAction::RecordEventLAPPD(): "
          //<< "No hits on LAPPD or "
          //<< "issue in hit collection readout";
        //break;
      //}

      //G4ThreeVector localPos = hit->GetLocalPos();
      //G4ThreeVector worldPos = hit->GetWorldPos();
      //G4double      time     = hit->GetTime();
      ////G4cout << "x = " << worldPos.x() / cm << "; "
        ////<< "y = " << worldPos.y() / cm << "; "
        ////<< "z = " << worldPos.z() / cm << "; "
        ////<< "time = " << time / ns << G4endl;
      //analysisManager->FillH3(0, 
                              //worldPos.x() / cm, 
                              //worldPos.z() / cm, 
                              //time / ns);
      //analysisManager->FillH2(0, 
                              //worldPos.x() / cm, 
                              //worldPos.z() / cm);

      //// Fill ntuple
      //// ntuple id = 0: PMT hits
      //// ntuple id = 1: LAPPD hits
      //analysisManager->FillNtupleIColumn(1, 0, 
          //G4EventManager::GetEventManager()->GetConstCurrentEvent()
          //->GetEventID());
      //analysisManager->FillNtupleIColumn(1, 1, hit->GetPhotonProcessId());
      //analysisManager->FillNtupleDColumn(1, 2, hit->GetTime());
      //analysisManager->FillNtupleDColumn(1, 3, hit->GetWorldPos().x());
      //analysisManager->FillNtupleDColumn(1, 4, hit->GetWorldPos().z());
      //analysisManager->FillNtupleDColumn(1, 5, hit->GetPhotonEnergy());
      //analysisManager->AddNtupleRow(1);
    //}

    ////
    //// Print diagnostics
    //// 

    //G4int printModulo = 
      //G4RunManager::GetRunManager()->GetPrintProgress();
    //if ( printModulo==0 || ev->GetEventID() % printModulo != 0) return;

    //G4PrimaryParticle* primary = ev->GetPrimaryVertex(0)->GetPrimary(0);
    //G4cout << G4endl
      //<< ">>> Event " << ev->GetEventID() << " >>> Simulation truth : "
      //<< primary->GetG4code()->GetParticleName()
      //<< " " << primary->GetMomentum() << G4endl;

    //// Photodetector
    //G4cout << "LAPPD has " << n_hit << " hits." << G4endl;
    //for (G4int i=0;i<n_hit;i++)
    //{
      //SLArLAPPDHit * hit = (*hHC1)[i];
      //hit->Print();
    //}
  //}
//}

void SLArEventAction::RecordEventTarget(const G4Event* ev)
{
  G4HCofThisEvent* hce = ev->GetHCofThisEvent();
  if (fTargetHCollID == -4) return;
  else 
  {
    // recover analysis manager
    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
    SLArTankHitsCollection* hHC1 
      = static_cast<SLArTankHitsCollection*>(hce->GetHC(fTargetHCollID));

    SLArTankHit* hit = (*hHC1)[0];
    fTotEdep = hit->GetDepositedEnergy();

    SLArAnaMgr->GetEvent()->GetPrimaries().front()->SetTotalEdep(fTotEdep);

    G4TrajectoryContainer* trj_cont =  ev->GetTrajectoryContainer();
    if (trj_cont)
    {
      auto primaries = SLArAnaMgr->GetEvent()->GetPrimaries();
      TrajectoryVector* trj_vec = trj_cont->GetVector();

      for (auto const t : *trj_vec)
      {
        SLArTrajectory* SLArTrj = (SLArTrajectory*)t;
        if (SLArTrj->GetPDGEncoding()!=0)              // filter optical photons
        {
          // Copy relevant attributes into SLArEvTrajectory
          SLArEventTrajectory* evTrajectory = new SLArEventTrajectory();
          evTrajectory->SetParticleName(SLArTrj->GetParticleName());
          evTrajectory->SetPDGID(SLArTrj->GetPDGEncoding());
          evTrajectory->SetTrackID(SLArTrj->GetTrackID());
          evTrajectory->SetParentID(SLArTrj->GetParentID());
          evTrajectory->SetCreatorProcess(SLArTrj->GetCreatorProcess());
          evTrajectory->SetInitKineticEne(SLArTrj->GetInitialKineticEnergy());
          evTrajectory->SetTime(SLArTrj->GetTime()); 
          // store trajectory points
          size_t npoints = SLArTrj->GetPointEntries(); 
          size_t nedeps = SLArTrj->GetEdep().size();
          /*
           *if ( npoints != nedeps) {
           *  printf("SLArEventAction::RecordEventTarget WARNING:\n");
           *  printf("Nr of trajectory points != edep points (%lu - %lu)\n\n", 
           *      npoints, nedeps);
           *}
           */
          float edep = 0; 
          for (int n=0; n<SLArTrj->GetPointEntries(); n++) {
            (n == 0) ? edep = 0 : edep = SLArTrj->GetEdep().at(n-1);
            evTrajectory->RegisterPoint(
                SLArTrj->GetPoint(n)->GetPosition().getX(),
                SLArTrj->GetPoint(n)->GetPosition().getY(),
                SLArTrj->GetPoint(n)->GetPosition().getZ(),
                edep
                );
          }

          // find the right primary to associate the trajectory
          for (auto &primary : primaries) {
            if (SLArTrj->GetTrackID() == primary->GetTrackID() || 
                SLArTrj->GetParentID() == primary->GetTrackID()) {
              primary->RegisterTrajectory(evTrajectory);
            }
          }
        }  

      } 
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void SLArEventAction::RecordEventHodo(const G4Event* ev)
//{
  //G4HCofThisEvent* hce = ev->GetHCofThisEvent();

  //if (fHodoHCollID == -3) return;
  //else 
  //{
    //// Get hits collections 
    //SLArHodoHitsCollection* hHC1 
      //= static_cast<SLArHodoHitsCollection*>(hce->GetHC(fHodoHCollID));

    //if ( (!hHC1) ) 
    //{
      //G4ExceptionDescription msg;
      //msg << "Some of hits collections of this event not found." 
        //<< G4endl; 
      //G4Exception("SLArEventAction::RecordEventHodo",
          //"SLArCode001", JustWarning, msg);
      //return;
    //}   

    //// Fill histograms & ntuple

    //// Get analysis manager
    //SLArAnalysisManager* SLArAnaMgr = 
      //SLArAnalysisManager::Instance();

    //// Fill histograms

    //G4int n_hit = hHC1->entries();

    //for (G4int i=0;i<n_hit;i++)
    //{
      //SLArHodoHit* hit = (*hHC1)[i];
      //if (!hit) { 
        //G4cout << "SLArEventAction::RecordEventHodo(): "
          //<< "No hits on Hodo or "
          //<< "issue in hit collection readout";
        //break;
      //}

      //if (hit->GetDepositedEnergy() > 0.)
      //{
        ////hit->Print();
        //SLArAnaMgr->GetEvent()->GetHodoSystem()->RegisterHit(
            //new SLArEventHitHodo(
              //hit->GetTime(), 
              //hit->GetDepositedEnergy(), 
              //hit->GetHodoNumber(), 
              //hit->GetBarNumber())
            //);
      //}
    //}
    
  //}
//}
