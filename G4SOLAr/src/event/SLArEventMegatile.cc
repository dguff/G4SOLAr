/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventMegatile
 * @created     : Wed Aug 10, 2022 13:47:24 CEST
 */

#include "event/SLArEventMegatile.hh"
#include <SLArAnalysisManager.hh>

ClassImp(SLArEventMegatile)

SLArEventMegatile::SLArEventMegatile() 
  : fIdx(0), fIsActive(true), fNhits(0) {}

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

int SLArEventMegatile::ConfigModule(const SLArCfgMegaTile* cfg) {
  int ntiles = 0; 
  for (auto &cfgTile : cfg->GetConstMap()) {
    int idx_tile = cfgTile.second->GetIdx(); 
    fTilesMap.insert(
          std::make_pair(idx_tile, new SLArEventTile(idx_tile))
        );
    ++ntiles; 
  }

  return ntiles; 
}

SLArEventTile* SLArEventMegatile::CreateEventTile(const int tileIdx) 
{
  if (fTilesMap.count(tileIdx)) {
    printf("SLArEventMegatile::CreateEventTile(%i) WARNING: Tile nr %i already present in MegatTile %i register\n", tileIdx, tileIdx, fIdx);
    return fTilesMap.find(tileIdx)->second;
  }

  auto t_event = new SLArEventTile(); 
  t_event->SetIdx(tileIdx); 
  fTilesMap.insert( std::make_pair(tileIdx, t_event) );  

  return t_event;
}

int SLArEventMegatile::RegisterHit(SLArEventPhotonHit* hit) {
  int tile_idx = hit->GetTileIdx(); 
  SLArEventTile* tile_ev = nullptr;
  auto t_itr = fTilesMap.find(tile_idx); 
  if (t_itr == fTilesMap.end()) tile_ev = CreateEventTile(tile_idx);
  else tile_ev = t_itr->second;

  tile_ev->RegisterHit(hit); 
  fNhits++; 
  return 1;
}

int SLArEventMegatile::GetNPhotonHits() const {
  int nhits = 0;
  for (auto &tile : fTilesMap) {
    nhits += tile.second->GetNhits(); 
  }

  return nhits; 
}

int SLArEventMegatile::GetNChargeHits() const {
  int nhits = 0;
  for (auto &tile : fTilesMap) {
    nhits += tile.second->GetPixelHits(); 
  }

  return nhits; 
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
