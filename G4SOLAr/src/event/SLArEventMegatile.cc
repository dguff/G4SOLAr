/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventMegatile
 * @created     : Wed Aug 10, 2022 13:47:24 CEST
 */

#include <memory>
#include "event/SLArEventMegatile.hh"
#include <SLArAnalysisManager.hh>

templateClassImp(SLArEventMegatile)

template class SLArEventMegatile<SLArEventTilePtr*>;
template class SLArEventMegatile<std::unique_ptr<SLArEventTileUniquePtr>>;

template<class T>
SLArEventMegatile<T>::SLArEventMegatile() 
  : fIdx(0), fIsActive(true), fNhits(0), 
    fLightBacktrackerRecordSize(0), fChargeBacktrackerRecordSize(0)
{}

template<>
SLArEventMegatileUniquePtr::SLArEventMegatile(const SLArEventMegatile& right) 
  : TNamed(right) 
{
  fIdx = right.fIdx; 
  fNhits = right.fNhits; 
  fIsActive = right.fIsActive; 
  fLightBacktrackerRecordSize = right.fLightBacktrackerRecordSize;
  fChargeBacktrackerRecordSize = right.fChargeBacktrackerRecordSize;
  for (const auto &evtile : right.fTilesMap) {
    fTilesMap[evtile.first] = std::make_unique<SLArEventTileUniquePtr>(*evtile.second);
  }
}


template<>
SLArEventMegatilePtr::SLArEventMegatile(const SLArEventMegatile& right) 
  : TNamed(right) 
{
  fIdx = right.fIdx; 
  fNhits = right.fNhits; 
  fIsActive = right.fIsActive; 
  fLightBacktrackerRecordSize = right.fLightBacktrackerRecordSize;
  fChargeBacktrackerRecordSize = right.fChargeBacktrackerRecordSize;
  for (const auto &evtile : right.fTilesMap) {
    fTilesMap[evtile.first] = new SLArEventTilePtr(*evtile.second);
  }
}

template<class T>
SLArEventMegatile<T>::SLArEventMegatile(SLArCfgMegaTile* cfg) 
  : fIdx(0), fIsActive(true), fNhits(0) 
{
  SetIdx(cfg->GetIdx());
  SetName(cfg->GetName());
  //ConfigModule(cfg); 
}


template<>
template<>
void SLArEventMegatileUniquePtr::SoftCopy(SLArEventMegatilePtr& record) const
{
  record.SoftResetHits(); 
  record.SetName( fName ); 
  record.SetActive( fIsActive ); 
  record.SetIdx( fIdx ); 
  record.SetLightBacktrackerRecordSize( fLightBacktrackerRecordSize ); 
  record.SetChargeBacktrackerRecordSize( fChargeBacktrackerRecordSize ); 
  
  for (const auto &tile : fTilesMap) {
    record.GetTileMap()[tile.first] = new SLArEventTilePtr();
    tile.second->SoftCopy( *record.GetTileMap()[tile.first] );
  }

  return;
}


template<>
int SLArEventMegatilePtr::ResetHits() {
  int nhits = 0;
  for (auto &tile : fTilesMap) {
    nhits += tile.second->GetNhits(); 
    tile.second->ResetHits(); 
  }

  fTilesMap.clear();
  
  return nhits; 
}

template<>
int SLArEventMegatileUniquePtr::ResetHits() {
  int nhits = 0;
  for (auto &tile : fTilesMap) {
    nhits += tile.second->GetNhits(); 
    tile.second->ResetHits(); 
  }

  fTilesMap.clear();
  
  return nhits; 
}

template<class T>
int SLArEventMegatile<T>::SoftResetHits() {
  int nhits = 0;
  for (auto &tile : fTilesMap) {
    nhits += tile.second->GetNhits(); 
    tile.second->SoftResetHits(); 
  }

  fTilesMap.clear();
  
  return nhits; 
}

template<>
SLArEventMegatileUniquePtr::~SLArEventMegatile()
{
  ResetHits();
  for (auto &evtile : fTilesMap) {
    evtile.second->ResetHits();
  }
  fTilesMap.clear(); 
}

template<>
SLArEventMegatilePtr::~SLArEventMegatile()
{
  ResetHits();
  for (auto &evtile : fTilesMap) {
    evtile.second->ResetHits();
    delete evtile.second;
  }
  fTilesMap.clear(); 
}

template<>
int SLArEventMegatileUniquePtr::ConfigModule(const SLArCfgMegaTile* cfg) {
  int ntiles = 0; 
  for (auto &cfgTile : cfg->GetConstMap()) {
    int idx_tile = cfgTile.second->GetIdx(); 
    std::unique_ptr<SLArEventTileUniquePtr> evtile = std::make_unique<SLArEventTileUniquePtr>(idx_tile);
    fTilesMap.insert(std::make_pair(idx_tile, std::move(evtile)));
    ++ntiles; 
  }

  return ntiles; 
}

template<>
int SLArEventMegatilePtr::ConfigModule(const SLArCfgMegaTile* cfg) {
  int ntiles = 0; 
  for (auto &cfgTile : cfg->GetConstMap()) {
    int idx_tile = cfgTile.second->GetIdx(); 
    SLArEventTilePtr* evtile = new SLArEventTilePtr(idx_tile);
    fTilesMap.insert(std::make_pair(idx_tile, std::move(evtile)));
    ++ntiles; 
  }

  return ntiles; 
}

template<>
std::unique_ptr<SLArEventTileUniquePtr>& SLArEventMegatileUniquePtr::GetOrCreateEventTile(const int& tileIdx) 
{
  auto it  = fTilesMap.find(tileIdx); 
  if (it != fTilesMap.end()) {
    //printf("SLArEventMegatile::CreateEventTile(%i) WARNING: Tile nr %i already present in MegatTile %i register\n", tileIdx, tileIdx, fIdx);
    return fTilesMap.find(tileIdx)->second;
  }
  else {
    std::unique_ptr<SLArEventTileUniquePtr> t_event = std::make_unique<SLArEventTileUniquePtr>();
    t_event->SetIdx(tileIdx); 
    t_event->SetBacktrackerRecordSize( fLightBacktrackerRecordSize ); 
    t_event->SetChargeBacktrackerRecordSize( fChargeBacktrackerRecordSize ); 
    fTilesMap.insert( std::make_pair(tileIdx, std::move(t_event) ) );  
    return fTilesMap[tileIdx];
  }
}

template<>
SLArEventTilePtr*& SLArEventMegatilePtr::GetOrCreateEventTile(const int& tileIdx) 
{
  auto it  = fTilesMap.find(tileIdx); 
  if (it != fTilesMap.end()) {
    //printf("SLArEventMegatile::CreateEventTile(%i) WARNING: Tile nr %i already present in MegatTile %i register\n", tileIdx, tileIdx, fIdx);
    return fTilesMap.find(tileIdx)->second;
  }
  else {
    SLArEventTilePtr* t_event = new SLArEventTilePtr();
    t_event->SetIdx(tileIdx); 
    t_event->SetBacktrackerRecordSize( fLightBacktrackerRecordSize ); 
    t_event->SetChargeBacktrackerRecordSize( fChargeBacktrackerRecordSize ); 
    fTilesMap.insert( std::make_pair(tileIdx, std::move(t_event) ) );  
    return fTilesMap[tileIdx];
  }
}

template<class T>
T& SLArEventMegatile<T>::RegisterHit(const SLArEventPhotonHit& hit) {
  int tile_idx = hit.GetTileIdx(); 
  fNhits++; 

  auto& tile_ev = GetOrCreateEventTile(tile_idx);
  tile_ev->RegisterHit(hit);
  return fTilesMap[tile_idx];
}

template<class T>
int SLArEventMegatile<T>::GetNPhotonHits() const {
  int nhits = 0;
  for (const auto &tile : fTilesMap) {
    nhits += tile.second->GetNhits(); 
  }

  return nhits; 
}

template<class T>
int SLArEventMegatile<T>::GetNChargeHits() const {
  int nhits = 0;
  for (const auto &tile : fTilesMap) {
    nhits += tile.second->GetPixelHits(); 
  }

  return nhits; 
}


template<class T>
void SLArEventMegatile<T>::SetActive(bool is_active) {
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
