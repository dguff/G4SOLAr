/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventMegatile
 * @created     : mercoledì ago 10, 2022 13:47:24 CEST
 */

#include "event/SLArEventMegatile.hh"

ClassImp(SLArEventMegatile)

SLArEventMegatile::SLArEventMegatile() 
  : fIdx(0), fIsActive(true), fNhits(0) {}; 

SLArEventMegatile::SLArEventMegatile(const SLArEventMegatile& right) 
  : TNamed(right) 
{
  fIdx = right.fIdx; 
  fNhits = right.fNhits; 
  fIsActive = right.fIsActive; 
  for (const auto &evtile : right.fTilesMap) {
    fTilesMap.insert(
        std::make_pair(evtile.first, new SLArEventTile(*evtile.second))
        );
  }
}

SLArEventMegatile::SLArEventMegatile(SLArCfgMegaTile* cfg) 
  : fIdx(0), fIsActive(true), fNhits(0) 
{
  SetIdx(cfg->GetIdx());
  SetName(cfg->GetName());
  ConfigModule(cfg); 
}

SLArEventMegatile::~SLArEventMegatile()
{
  for (auto &evtile : fTilesMap) {
    delete evtile.second; evtile.second = nullptr; 
  }
  fTilesMap.clear(); 
}

int SLArEventMegatile::ConfigModule(SLArCfgMegaTile* cfg) {
  int ntiles = 0; 
  for (const auto &cfgTile : cfg->GetMap()) {
    int idx_tile = cfgTile.second->GetIdx(); 
    fTilesMap.insert(
          std::make_pair(idx_tile, new SLArEventTile(idx_tile))
        );
    ++ntiles; 
  }

  return ntiles; 
}

int SLArEventMegatile::RegisterHit(SLArEventPhotonHit* hit) {
  int tile_idx = hit->GetTileIdx(); 
  if (fTilesMap.count(tile_idx)) {
    fTilesMap.find(tile_idx)->second->RegisterHit(hit); 
    fNhits++; 
    return 1; 
  } else {
    return 0; 
  }
}

int SLArEventMegatile::ResetHits() {
  int nhits = 0;
  for (auto &tile : fTilesMap) {
    nhits += tile.second->GetNhits(); 
    tile.second->ResetHits(); 
  }

  return nhits; 
}

void SLArEventMegatile::SetActive(bool is_active) {
  for (auto &tile : fTilesMap) {
    tile.second->SetActive(is_active); 
  } 
  return;
}

bool SLArEventMegatile::SortHits() {
  for (auto &tile : fTilesMap) {
    tile.second->SortHits(); 
  }

  return true;
}
