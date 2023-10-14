/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventMegatile
 * @created     : Wed Aug 10, 2022 13:47:24 CEST
 */

#include <memory>
#include "event/SLArEventMegatile.hh"
#include <SLArAnalysisManager.hh>

ClassImp(SLArEventMegatile)


SLArEventMegatile::SLArEventMegatile() 
  : fIdx(0), fIsActive(true), fNhits(0), 
    fLightBacktrackerRecordSize(0), fChargeBacktrackerRecordSize(0)
{}

SLArEventMegatile::SLArEventMegatile(const SLArEventMegatile& right) 
  : TNamed(right) 
{
  fIdx = right.fIdx; 
  fNhits = right.fNhits; 
  fIsActive = right.fIsActive; 
  fLightBacktrackerRecordSize = right.fLightBacktrackerRecordSize;
  fChargeBacktrackerRecordSize = right.fChargeBacktrackerRecordSize;
  for (const auto &evtile : right.fTilesMap) {
    fTilesMap[evtile.first] = evtile.second;
  }
}


SLArEventMegatile::SLArEventMegatile(SLArCfgMegaTile* cfg) 
  : SLArEventMegatile()
{
  SetIdx(cfg->GetIdx());
  SetName(cfg->GetName());
  //ConfigModule(cfg); 
}


int SLArEventMegatile::ResetHits() {
  int nhits = 0;
  for (auto &tile : fTilesMap) {
    nhits += tile.second->GetNhits(); 
    tile.second->ResetHits(); 
    delete tile.second;
  }

  fTilesMap.clear();
  
  return nhits; 
}


SLArEventMegatile::~SLArEventMegatile()
{
  ResetHits();
  fTilesMap.clear(); 
}

int SLArEventMegatile::ConfigModule(const SLArCfgMegaTile* cfg) {
  int ntiles = 0; 
  for (auto &cfgTile : cfg->GetConstMap()) {
    int idx_tile = cfgTile.second->GetIdx(); 
    fTilesMap.insert(std::make_pair(idx_tile, new SLArEventTile(idx_tile) ));
    ++ntiles; 
  }

  return ntiles; 
}


SLArEventTile* SLArEventMegatile::GetOrCreateEventTile(const int& tileIdx) 
{
  auto it  = fTilesMap.find(tileIdx); 
  if (it != fTilesMap.end()) {
    //printf("SLArEventMegatile::CreateEventTile(%i) WARNING: Tile nr %i already present in MegatTile %i register\n", tileIdx, tileIdx, fIdx);
    return fTilesMap.find(tileIdx)->second;
  }
  else {
    fTilesMap.insert( std::make_pair(tileIdx, new SLArEventTile(tileIdx) ) );  
    auto& t_event = fTilesMap[tileIdx];
    t_event->SetBacktrackerRecordSize( fLightBacktrackerRecordSize ); 
    t_event->SetChargeBacktrackerRecordSize( fChargeBacktrackerRecordSize ); 
    return t_event;
  }
}


SLArEventTile* SLArEventMegatile::RegisterHit(const SLArEventPhotonHit& hit) {
  int tile_idx = hit.GetTileIdx(); 
  fNhits++; 

  auto tile_ev = GetOrCreateEventTile(tile_idx);
  tile_ev->RegisterHit(hit);
  return tile_ev;
}

int SLArEventMegatile::GetNPhotonHits() const {
  int nhits = 0;
  for (const auto &tile : fTilesMap) {
    nhits += tile.second->GetNhits(); 
  }

  return nhits; 
}

int SLArEventMegatile::GetNChargeHits() const {
  int nhits = 0;
  for (const auto &tile : fTilesMap) {
    nhits += tile.second->GetPixelHits(); 
  }

  return nhits; 
}


void SLArEventMegatile::SetActive(bool is_active) {
  for (auto &tile : fTilesMap) {
    tile.second->SetActive(is_active); 
  } 
  return;
}

//bool SLArEventMegatile::SortHits() {
  //for (auto &tile : fTilesMap) {
    //tile.second->SortHits(); 
  //}

  //return true;
//}
