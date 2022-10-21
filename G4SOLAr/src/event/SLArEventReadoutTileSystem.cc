/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventReadoutTileSystem
 * @created     : mercoledì ago 10, 2022 14:39:21 CEST
 */

#include "event/SLArEventReadoutTileSystem.hh"

ClassImp(SLArEventReadoutTileSystem)

SLArEventReadoutTileSystem::SLArEventReadoutTileSystem()
  : fNhits(0), fIsActive(true) {}

SLArEventReadoutTileSystem::SLArEventReadoutTileSystem(const SLArEventReadoutTileSystem& right) 
  : TNamed(right) 
{
  fNhits = right.fNhits;
  fIsActive = right.fIsActive; 
  for (const auto &mgev : right.fMegaTilesMap) {
    fMegaTilesMap.insert(
        std::make_pair(mgev.first, (SLArEventMegatile*)mgev.second->Clone())
        );
  }
}

SLArEventReadoutTileSystem::SLArEventReadoutTileSystem(SLArCfgPixSys* cfg) {
  SetName(cfg->GetName()); 
  ConfigSystem(cfg); 
  return;
}

SLArEventReadoutTileSystem::~SLArEventReadoutTileSystem() {
  for (auto &mgtile : fMegaTilesMap) {
    delete mgtile.second; mgtile.second = nullptr;
  }
  fMegaTilesMap.clear(); 
}

int SLArEventReadoutTileSystem::ConfigSystem(SLArCfgPixSys* cfg) {
  int imegatile = 0; 
  for (const auto &mtcfg : cfg->GetModuleMap()) {
    int megatile_idx = mtcfg.second->GetIdx(); 
    if (fMegaTilesMap.count(megatile_idx) == 0) {
      fMegaTilesMap.insert(
          std::make_pair(megatile_idx, 
            new SLArEventMegatile(mtcfg.second))
          );
      imegatile++;
    }
  }

  return imegatile;
}

int SLArEventReadoutTileSystem::RegisterHit(SLArEventPhotonHit* hit) {
  int mgtile_idx = hit->GetMegaTileIdx(); 
  if (fMegaTilesMap.count(mgtile_idx)) {
    fMegaTilesMap.find(mgtile_idx)->second->RegisterHit(hit);
    fNhits++;
    return 1; 
  } else {
    printf("SLArEventReadoutTileSystem::RegisterHit WARNING\n"); 
    printf("Megatile with ID %i is not in store\n", mgtile_idx); 
    return 0; 
  }
}

int SLArEventReadoutTileSystem::ResetHits() {
  int nn = 0; 
  for (auto &mgtile : fMegaTilesMap) {
    nn += mgtile.second->ResetHits(); 
  }
  return nn; 
}

void SLArEventReadoutTileSystem::SetActive(bool is_active) {
  fIsActive = is_active; 
  for (auto &mgtile : fMegaTilesMap) {
    mgtile.second->SetActive(is_active); 
  }
}

bool SLArEventReadoutTileSystem::SortHits() {
  int isort = true;
  for (auto &mgtile : fMegaTilesMap) {
    isort *= mgtile.second->SortHits(); 
  }
  return isort;
}
