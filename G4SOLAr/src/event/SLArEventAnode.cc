/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventAnode.cc
 * @created     : mercoledì ago 10, 2022 14:39:21 CEST
 */

#include "event/SLArEventAnode.hh"

ClassImp(SLArEventAnode)

SLArEventAnode::SLArEventAnode()
  : fID(0), fNhits(0), fIsActive(true) {}

SLArEventAnode::SLArEventAnode(const SLArEventAnode& right) 
  : TNamed(right) 
{
  fID = right.fID; 
  fNhits = right.fNhits;
  fIsActive = right.fIsActive; 
  for (const auto &mgev : right.fMegaTilesMap) {
    fMegaTilesMap.insert(
        std::make_pair(mgev.first, (SLArEventMegatile*)mgev.second->Clone())
        );
  }
}

SLArEventAnode::SLArEventAnode(SLArCfgAnode* cfg) {
  SetName(cfg->GetName()); 
  ConfigSystem(cfg); 
  return;
}

SLArEventAnode::~SLArEventAnode() {
  for (auto &mgtile : fMegaTilesMap) {
    delete mgtile.second; mgtile.second = nullptr;
  }
  fMegaTilesMap.clear(); 
}

int SLArEventAnode::ConfigSystem(SLArCfgAnode* cfg) {
  int imegatile = 0; 
  fID = cfg->GetIdx(); 
  for (const auto &mtcfg : cfg->GetMap()) {
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

int SLArEventAnode::RegisterHit(SLArEventPhotonHit* hit) {
  int mgtile_idx = hit->GetMegaTileIdx(); 
  if (fMegaTilesMap.count(mgtile_idx)) {
    fMegaTilesMap.find(mgtile_idx)->second->RegisterHit(hit);
    fNhits++;
    return 1; 
  } else {
    printf("SLArEventAnode::RegisterHit WARNING\n"); 
    printf("Megatile with ID %i is not in store\n", mgtile_idx); 
    return 0; 
  }
}

int SLArEventAnode::ResetHits() {
  int nn = 0; 
  for (auto &mgtile : fMegaTilesMap) {
    nn += mgtile.second->ResetHits(); 
  }
  return nn; 
}

void SLArEventAnode::SetActive(bool is_active) {
  fIsActive = is_active; 
  for (auto &mgtile : fMegaTilesMap) {
    mgtile.second->SetActive(is_active); 
  }
}

bool SLArEventAnode::SortHits() {
  int isort = true;
  for (auto &mgtile : fMegaTilesMap) {
    isort *= mgtile.second->SortHits(); 
  }
  return isort;
}
