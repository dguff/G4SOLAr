/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventAction.cc
 * @created     : giovedì nov 03, 2022 12:36:27 CET
 */

#include "SLArAnalysisManager.hh"
#include "SLArEventAction.hh"
#include "SLArRunAction.hh"
#include "SLArReadoutTileHit.hh"
#include "SLArSuperCellHit.hh"
#include "SLArTrajectory.hh"
#include "SLArDetectorConstruction.hh"
#include "detector/TPC/SLArLArHit.hh"
#include "physics/SLArElectronDrift.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"

#include "G4ios.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArEventAction::SLArEventAction()
: G4UserEventAction(), 
  fTileHCollID  (-2), 
  fSuperCellHCollID(-5)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  fHitCount                = 0;
  fPhotonCount_Scnt        = 0;
  fPhotonCount_Cher        = 0;
  fPhotonCount_WLS         = 0;
  fAbsorptionCount         = 0;
  fReadoutTileHits         = 0; 
  fSuperCellHits           = 0; 
  fBoundaryAbsorptionCount = 0;
  fTotEdep                 = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArEventAction::~SLArEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArEventAction::BeginOfEventAction(const G4Event*)
{

#ifdef SLAR_DEBUG
  printf("SLArEventAction::BeginOfEventAction()\n");
#endif

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    if (fTileHCollID == -2) 
      fTileHCollID  = sdManager->GetCollectionID("ReadoutTileColl"  );
    if (fSuperCellHCollID == -5) 
      fSuperCellHCollID = sdManager->GetCollectionID("SuperCellColl"); 
    if (fLArHCollID.empty()) {
      auto detConstruction = (SLArDetectorConstruction*)
        G4RunManager::GetRunManager()->GetUserDetectorConstruction(); 
      for (const auto &tpc : detConstruction->GetDetTPCs() ) {
        auto coll_id = 
          sdManager->GetCollectionID("TPC"+std::to_string(tpc.first)+"Coll");
        fLArHCollID.push_back(coll_id); 
      }
    }
     

#ifdef SLAR_DEBUG
    G4cout << "SLArEventAction::BeginOfEventAction(): ";
    G4cout << "ReadoutTile ID = " << fTileHCollID   << G4endl;
    G4cout << "SuperCell ID   = " << fSuperCellHCollID << G4endl;
    G4cout << "LAr volume ID  = "; 
    for (const auto &id : fLArHCollID) printf("%i \n", id);
    G4cout << G4endl;
#endif

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
    fReadoutTileHits         = 0; 
    fSuperCellHits           = 0; 

}     

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArEventAction::EndOfEventAction(const G4Event* event)
{
#ifdef SLAR_DEBUG
  printf("SLArEventAction::EndOfEventAction()\n"); 
#endif

    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (!hce) 
    {
        G4ExceptionDescription msg;
        msg << "No hits collection of this event found." << G4endl; 
        G4Exception("SLArEventAction::EndOfEventAction()",
                    "SLArCode001", JustWarning, msg);
        return;
    }   
    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();

    RecordEventLAr( event );

    if ( !SLArAnaMgr->GetAnodeCfg().empty() ) {
      RecordEventReadoutTile ( event );
    }

    if (SLArAnaMgr->GetPDSCfg()) {
      printf("Recording SuperCell hits...\n");
      RecordEventSuperCell( event );
      printf("DONE\n");
    }

     
    SLArAnaMgr->GetEvent()->SetEvNumber(event->GetEventID());
    
    SLArAnaMgr->FillEvTree();

    printf("SLArEventAction::EndOfEventAction()\n"); 
    printf("OpticalPhoton Monitor:\nCherenkov: %i\nScintillation: %i\n\n", 
        fPhotonCount_Cher, fPhotonCount_Scnt);
    printf("Primary particles:\n");
    for (const auto &p : SLArAnaMgr->GetEvent()->GetPrimaries()) {
      printf("%s - %g MeV - trk ID %i\n", 
          p->GetParticleName().Data(), p->GetEnergy(), p->GetTrackID());
      printf("\t%i scintillation ph\n\t%i Cerenkov photons\n", 
          p->GetTotalScintPhotons(), p->GetTotalCerenkovPhotons()); 
      printf("ReadoutTile Hits: %i\nSuperCell Hits: %i\n\n", 
          fReadoutTileHits, fSuperCellHits);
    }

    fParentIDMap.clear(); 

    SLArAnaMgr->GetEvent()->Reset();
}

void SLArEventAction::RecordEventReadoutTile(const G4Event* ev)
{

  G4HCofThisEvent* hce = ev->GetHCofThisEvent();

  if (fTileHCollID!= -2) 
  {
    // Get hits collections 
    SLArReadoutTileHitsCollection* hHC1 
      = static_cast<SLArReadoutTileHitsCollection*>(hce->GetHC(fTileHCollID));

    if ( (!hHC1) ) 
    {
      G4ExceptionDescription msg;
      msg << "Some of hits collections of this event not found." 
        << G4endl; 
      G4Exception("SLArEventAction::RecordEventReadoutTile",
          "SLArCode001", JustWarning, msg);
      return;
    }   

    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();

    // Fill histograms
    G4int n_hit = hHC1->entries();

    for (G4int i=0;i<n_hit;i++) {
      SLArReadoutTileHit* hit = (*hHC1)[i];
      if (!hit) { 
#ifdef SLAR_DEBUG
        G4cout << "SLArEventAction::RecordEventReadoutTile(): "
          << "No hits on SiPMs or "
          << "issue in hit collection readout";
#endif

        break;
      }

      G4ThreeVector localPos = hit->GetLocalPos();
      G4ThreeVector worldPos = hit->GetWorldPos();
      G4double      time     = hit->GetTime();
      G4double      wavelen  = hit->GetPhotonWavelength(); 
      G4int         anode_idx = hit->GetAnodeIdx();
      G4int         mtrow_nr  = hit->GetRowMegaTileIdx(); 
      G4int         mgtile_nr = hit->GetMegaTileIdx(); 
      G4int         rowtile_nr = hit->GetRowTileIdx(); 
      G4int         tile_nr    = hit->GetTileIdx(); 

#ifdef SLAR_DEBUG
      G4cout << "SLArEventAction::RecordEventReadoutTile()" << G4endl;
      printf("Tile idx [%i, %i, %i, %i]\n", mtrow_nr, mgtile_nr, rowtile_nr, tile_nr);
      G4cout << "x    = " << G4BestUnit(worldPos.x(), "Length") << "; "
             << "y    = " << G4BestUnit(worldPos.y(), "Length") << "; "
             << "time = " << G4BestUnit(time, "Time") << G4endl;
#endif
      
      SLArEventPhotonHit* dstHit = new SLArEventPhotonHit(
          time, 
          hit->GetPhotonProcessId(), 
          wavelen);
      dstHit->SetLocalPos(localPos.x(), localPos.y(), localPos.z());
      dstHit->SetTileInfo(mtrow_nr, mgtile_nr, rowtile_nr, tile_nr); 
      dstHit->SetRowCellNr(hit->GetRowCellNr()); 
      dstHit->SetCellNr(hit->GetCellNr()); 

      SLArAnaMgr->GetEvent()->GetEventAnodeByID(anode_idx)->RegisterHit(
                            (SLArEventPhotonHit*)dstHit->Clone());
      
      delete dstHit;
    }

    // Sort hits on PMTs
    for (auto &evAnode : SLArAnaMgr->GetEvent()->GetEventAnode()) {
      evAnode.second->SortHits();
    }
    

    // Print diagnostics
    G4int printModulo = 
      G4RunManager::GetRunManager()->GetPrintProgress();
    if ( printModulo==0 || ev->GetEventID() % printModulo != 0) return;
  }

  printf("SLArEventAction::RecordEventReadoutTile() DONE\n");
}

void SLArEventAction::RecordEventSuperCell(const G4Event* ev)
{

  G4HCofThisEvent* hce = ev->GetHCofThisEvent();
  if (fSuperCellHCollID != -5) 
  {
    // Get hits collections 
    SLArSuperCellHitsCollection* hHC1 
      = static_cast<SLArSuperCellHitsCollection*>(hce->GetHC(fSuperCellHCollID));

    if ( (!hHC1) ) 
    {
      G4ExceptionDescription msg;
      msg << "Some of hits collections of this event not found." 
        << G4endl; 
      G4Exception("SLArEventAction::RecordEventSuperCell",
          "SLArCode001", JustWarning, msg);
      return;
    }   
    SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
    G4int n_hit = hHC1->entries();
    for (G4int i=0;i<n_hit;i++) {
      SLArSuperCellHit* hit = (*hHC1)[i];
      if (!hit) { 
#ifdef SLAR_DEBUG
        G4cout << "SLArEventAction::RecordEventSuperCell(): "
          << "No hits on SuperCell or "
          << "issue in hit collection readout";
#endif

        break;
      }

      G4ThreeVector localPos = hit->GetLocalPos();
      G4ThreeVector worldPos = hit->GetWorldPos();
      G4double      time     = hit->GetTime();
      G4double      wavelen  = hit->GetPhotonWavelength(); 
      //G4int         id       = hit->GetSuperCellIdx(); 
      G4int         array_nr = hit->GetSuperCellArrayNo(); 
      G4int         cellrow_nr = hit->GetSuperCellRowNo(); 
      G4int         cell_nr    = hit->GetSuperCellNo(); 


#ifdef SLAR_DEBUG
      G4cout << "SLArEventAction::RecordEventSuperCell()" << G4endl;
      printf("SuperCell id [%i, %i, %i]\n", cell_nr, cellrow_nr, array_nr);
      G4cout << "x    = " << G4BestUnit(worldPos.x(), "Length") << "; "
             << "y    = " << G4BestUnit(worldPos.y(), "Length") << "; "
             << "time = " << G4BestUnit(time, "Time") << G4endl;
#endif
      
      SLArEventPhotonHit* dstHit = new SLArEventPhotonHit(
          time, 
          hit->GetPhotonProcessId(), 
          wavelen);
      dstHit->SetLocalPos(localPos.x(), localPos.y(), localPos.z());
      dstHit->SetTileInfo(0, array_nr, cellrow_nr, cell_nr); 

      SLArAnaMgr->GetEvent()->GetEventSuperCellArray(array_nr)->RegisterHit(
                            (SLArEventPhotonHit*)dstHit->Clone());
      
      delete dstHit;
    }

    // Sort hits on PMTs
    printf("Sorting hits...\n"); 
    for (auto &evSCArray : SLArAnaMgr->GetEvent()->GetEventSuperCellArray()) {
      evSCArray.second->SortHits(); 
    }

    // Print diagnostics
    G4int printModulo = 
      G4RunManager::GetRunManager()->GetPrintProgress();
    if ( printModulo==0 || ev->GetEventID() % printModulo != 0) return;
  }

  printf("SLArEventAction::RecordEventSuperCell() DONE\n");
}


void SLArEventAction::RecordEventLAr(const G4Event* ev)
{
#ifdef SLAR_DEBUG
  printf("  -> RecordEventLAr()\n");
#endif

  G4HCofThisEvent* hce = ev->GetHCofThisEvent();
  if (fLArHCollID.empty()) return;
  else 
  {
    // recover analysis manager
    //SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
    for (const auto &id : fLArHCollID) {
      SLArLArHitsCollection* hHC1 
        = static_cast<SLArLArHitsCollection*>(hce->GetHC(id));

      SLArLArHit* hit = (*hHC1)[0];
      fTotEdep = hit->GetDepositedEnergy();
    }
/*
 *    G4TrajectoryContainer* trj_cont =  ev->GetTrajectoryContainer();
 *    if (trj_cont)
 *    {
 *      auto primaries = SLArAnaMgr->GetEvent()->GetPrimaries();
 *      TrajectoryVector* trj_vec = trj_cont->GetVector();
 *
 *      for (auto const t : *trj_vec)
 *      {
 *        SLArTrajectory* SLArTrj = (SLArTrajectory*)t;
 *        // filter optical photons
 *        // NOTE that in Geant4 v11.0.1 OpticalPhoton PDG encoding is -22
 *        if (SLArTrj->GetPDGEncoding()!=0 && SLArTrj->GetPDGEncoding() != -22)
 *        {
 *
 *          // Copy relevant attributes into SLArEvTrajectory
 *          SLArEventTrajectory* evTrajectory = new SLArEventTrajectory();
 *          evTrajectory->SetParticleName(SLArTrj->GetParticleName());
 *          evTrajectory->SetPDGID(SLArTrj->GetPDGEncoding());
 *          evTrajectory->SetTrackID(SLArTrj->GetTrackID());
 *          evTrajectory->SetParentID(SLArTrj->GetParentID());
 *          evTrajectory->SetCreatorProcess(SLArTrj->GetCreatorProcess());
 *          evTrajectory->SetInitKineticEne(SLArTrj->GetInitialKineticEnergy());
 *          evTrajectory->SetTime(SLArTrj->GetTime()); 
 *
 *#ifdef SLAR_DEBUG
 *  printf("%*cRecording trk %i to register: PGD ID %i (%s) -- %i points\n", 6, ' ', 
 *      SLArTrj->GetTrackID(), 
 *      SLArTrj->GetPDGEncoding(), 
 *      SLArTrj->GetParticleName().c_str(),
 *      SLArTrj->GetPointEntries()); 
 *#endif
 *          // store trajectory points
 *          size_t npoints = SLArTrj->GetPointEntries(); 
 *          size_t nedeps = SLArTrj->GetEdep().size();
 *          if ( npoints != nedeps+1) {
 *            printf("SLArEventAction::RecordEventTarget WARNING:\n");
 *            printf("Nr of trajectory points != edep points (%lu - %lu)\n\n", 
 *                npoints, nedeps);
 *            //for (int iip =0; iip < SLArTrj->GetPointEntries(); iip++) {
 *                //double x = SLArTrj->GetPoint(iip)->GetPosition().getX();
 *                //double y = SLArTrj->GetPoint(iip)->GetPosition().getY();
 *                //double z = SLArTrj->GetPoint(iip)->GetPosition().getZ();
 *                //printf("Trj points: [%.2f, %.2f, %.2f]\n", 
 *                    //x/CLHEP::m, y/CLHEP::m, z/CLHEP::m);
 *            //}
 *          }
 *          G4double trj_edep = 0; 
 *          for (const auto &edep : SLArTrj->GetEdep()) trj_edep += edep; 
 *          float edep = 0; 
 *          int   n_ph = 0; 
 *          int   n_el = 0; 
 *          int   copyNo = 0; 
 *          for (int n=0; n<SLArTrj->GetPointEntries(); n++) {
 *            if (n == 0) {
 *              edep = 0; n_ph = 0; n_el = 0; copyNo = 0;
 *            } else {
 *              edep = SLArTrj->GetEdep().at(n-1);
 *              n_ph = SLArTrj->GetNphotons().at(n-1); 
 *              n_el = SLArTrj->GetIonElectrons().at(n-1);
 *              copyNo = SLArTrj->GetVolCopyNumumber().at(n-1); 
 *            } 
 *
 *            evTrajectory->RegisterPoint(
 *                SLArTrj->GetPoint(n)->GetPosition().getX(),
 *                SLArTrj->GetPoint(n)->GetPosition().getY(),
 *                SLArTrj->GetPoint(n)->GetPosition().getZ(),
 *                edep, n_ph, n_el, copyNo
 *                );
 *           
 *          } // end of trj points loop
 *
 *          // find the right primary to associate the trajectory
 *          for (auto &primary : primaries) {
 *            if (SLArTrj->GetTrackID() == primary->GetTrackID() || 
 *                SLArTrj->GetParentID() == primary->GetTrackID()) {
 *              primary->RegisterTrajectory(evTrajectory);
 *            } else {
 *              auto slar_trjs = primary->GetTrajectories(); 
 *              for (const auto &strj : slar_trjs) {
 *                if (strj->GetTrackID() == SLArTrj->GetParentID()) {
 *                  primary->RegisterTrajectory(evTrajectory); 
 *                }
 *              }
 *            }
 *          }    
 *        }  
 *
 *      } 
 *    }
 */
  }


#ifdef SLAR_DEBUG
  printf("     DONE\n"); 
#endif
}

void SLArEventAction::RegisterNewTrackPID(int trk_id, int p_id) {
  fParentIDMap.insert( std::make_pair(trk_id, p_id) ); 
  return;
}

int SLArEventAction::FindTopParentID(int trkid) {
  int primary = -1; 
  int pid = trkid; 

  SLArAnalysisManager* anaMngr = SLArAnalysisManager::Instance(); 
  auto primaries = anaMngr->GetEvent()->GetPrimaries(); 
  bool caught = false; 
//#ifdef SLAR_DEBUG
  //printf("Lookging for primary parent of %i among\n", trkid);
  //for (const auto& _pid : fParentIDMap) {
    //printf("%i - PID: %i\n", _pid.first, _pid.second); 
  //}

  //printf("\nList of primaries: \n");
  //for (const auto &p : primaries) {
    //printf("%s - PID: %i\n", p->GetParticleName().Data(), p->GetTrackID());
  //}
  //getchar(); 
//#endif

  while ( !caught ) {
    pid = fParentIDMap[trkid];
//#ifdef SLAR_DEBUG
    //printf("local parent id: %i\n", pid);
//#endif

    for (const auto &p : primaries) {
      if (pid == p->GetTrackID()) {
        primary = pid; 
        caught = true;
      }
    }

    trkid = pid; 
//#ifdef SLAR_DEBUG
    //getchar(); 
//#endif
  }

//#ifdef SLAR_DEBUG
  //printf("Caught! returning %i\n", primary);
  //getchar(); 
//#endif

  return primary; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

