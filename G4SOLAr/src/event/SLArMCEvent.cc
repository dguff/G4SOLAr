/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArMCEvent
 * @created     : mercoledì ago 10, 2022 11:52:40 CEST
 */

#include "event/SLArMCEvent.hh"
#include "TRandom3.h"
ClassImp(SLArMCEvent)

SLArMCEvent::SLArMCEvent() : 
  fEvNumber(0), fDirection{0, 0, 0},
  fEvSystemTile(nullptr), fEvSystemSuperCell(nullptr)
{
  fEvSystemTile = new SLArEventReadoutTileSystem();
  fEvSystemSuperCell = new SLArEventSuperCellSystem(); 
}

SLArMCEvent::~SLArMCEvent()
{
  std::cerr << "Deleting SLArMCEvent..." << std::endl;
  if (fEvSystemTile)  delete fEvSystemTile; 
  if (fEvSystemSuperCell) delete fEvSystemSuperCell; 
  for (auto &p : fSLArPrimary) {
    delete p; p = nullptr; 
  }
  fSLArPrimary.clear();
  std::cerr << "SLArMCEvent DONE" << std::endl;
}

int SLArMCEvent::ConfigReadoutTileSystem(SLArCfgSystemPix* pixSysCfg)
{
  if (!fEvSystemTile) {
    std::cout << "SLArMCEvent::ConfigReadoutTileSystem: fEvSystemTile is null!"
              << std::endl;
    return 0;
  }

  fEvSystemTile->ConfigSystem(pixSysCfg);
  return fEvSystemTile->GetMegaTilesMap().size();
}

int SLArMCEvent::ConfigSuperCellSystem(SLArCfgSystemSuperCell* supercellSysCfg)
{
  if (!fEvSystemSuperCell) {
    std::cout << "SLArMCEvent::ConfigSuperCellSystem: fEvSystemSuperCell is null!"
              << std::endl;
    return 0;
  }

  fEvSystemSuperCell->ConfigSystem(supercellSysCfg);
  return fEvSystemSuperCell->GetSuperCellMap().size();
}


int SLArMCEvent::SetEvNumber(int nEv)
{
  fEvNumber = nEv;
  return fEvNumber;
}

void SLArMCEvent::Reset()
{
  if (fEvSystemTile ) fEvSystemTile->ResetHits();
  if (fEvSystemSuperCell) fEvSystemSuperCell->ResetHits(); 

  for (auto &p : fSLArPrimary) {
    delete p; p = nullptr; 
  }
  fSLArPrimary.clear(); 
  fDirection = {0, 0, 1};
  fEvNumber = -1;
}

void SLArMCEvent::SetDirection(double* dir) {
  if (dir) {
    fDirection.at(0) = dir[0];  
    fDirection.at(1) = dir[1];  
    fDirection.at(2) = dir[2];  
  } 
}

void SLArMCEvent::SetDirection(double px, double py, double pz) {
    fDirection.at(0) = px;  
    fDirection.at(1) = py;  
    fDirection.at(2) = pz;  
}

bool SLArMCEvent::CheckIfPrimary(int trkId) {
  bool is_primary = false; 
  for (const auto &p : fSLArPrimary) {
    if (trkId == p->GetTrackID()) {
      is_primary = true; 
      break;
    }
  }
  return is_primary; 
}
