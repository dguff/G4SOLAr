/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventSuperCellSystem
 * @created     : giovedì ott 20, 2022 16:16:03 CEST
 */

#include "event/SLArEventSuperCellSystem.hh"

ClassImp(SLArEventSuperCellSystem)

SLArEventSuperCellSystem::SLArEventSuperCellSystem()
  : TNamed(), fNhits(0), fIsActive(true) {}

SLArEventSuperCellSystem::SLArEventSuperCellSystem(const SLArEventSuperCellSystem& ev)
  : TNamed(ev) 
{
  fNhits = ev.fNhits; 
  fIsActive = ev.fIsActive; 
  for (const auto &sc : ev.fSuperCellMap) {
    fSuperCellMap.insert(
        std::make_pair(sc.first, (SLArEventSuperCell*)sc.second->Clone()));
  }
  return;
}

SLArEventSuperCellSystem::SLArEventSuperCellSystem(SLArCfgSCSys* cfg) {
  SetName(cfg->GetName()); 
  ConfigSystem(cfg); 
  return;
}

SLArEventSuperCellSystem::~SLArEventSuperCellSystem() {
  for (auto &scevent : fSuperCellMap) {
    delete scevent.second; scevent.second = nullptr;
  }
  fSuperCellMap.clear(); 
}

int SLArEventSuperCellSystem::ConfigSystem(SLArCfgSCSys* cfg) {
  int nsc = 0; 
  for (const auto &scarr : cfg->GetModuleMap()) {
    int array_idx = scarr.second->GetIdx(); 
    for (const auto &sc : scarr.second->GetMap()) {
      if (fSuperCellMap.count(sc.first) == 0) {
        fSuperCellMap.insert(
              std::make_pair(sc.first, new SLArEventSuperCell(sc.first))
            ); 
        nsc++;
      } 
    }
  }

  return nsc; 
}


int SLArEventSuperCellSystem::RegisterHit(SLArEventPhotonHit* hit) {
  int sc_idx = hit->GetTileNr(); 
  if (fSuperCellMap.count(sc_idx)) {
    fSuperCellMap.find(sc_idx)->second->RegisterHit(hit);
    fNhits++;
    return 1; 
  } else {
    printf("SLArEventSuperCellSystem::RegisterHit WARNING\n"); 
    printf("SuperCell with ID %i is not in store [%i,%i,%i]\n", 
        sc_idx, hit->GetMegaTileIdx(), hit->GetRowTileNr(), hit->GetTileNr()); 
    return 0; 
  }
}

int SLArEventSuperCellSystem::ResetHits() {
  int nn = 0; 
  for (auto &sc : fSuperCellMap) {
    nn += sc.second->ResetHits(); 
  }
  return nn; 
}

void SLArEventSuperCellSystem::SetActive(bool is_active) {
  fIsActive = is_active; 
  for (auto &sc : fSuperCellMap) {
    sc.second->SetActive(is_active); 
  }
}

bool SLArEventSuperCellSystem::SortHits() {
  int isort = true;
  for (auto &sc : fSuperCellMap) {
    isort *= sc.second->SortHits(); 
  }
  return isort;
}













