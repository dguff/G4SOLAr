/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventAnode.cc
 * @created     : mercoledì ago 10, 2022 14:39:21 CEST
 */

#include "event/SLArEventAnode.hh"
#include "config/SLArCfgMegaTile.hh"

ClassImp(SLArEventAnode)

SLArEventAnode::SLArEventAnode()
  : fID(0), fNhits(0), fIsActive(true), 
    fLightBacktrackerRecordSize(0), fChargeBacktrackerRecordSize(0) 
{}

SLArEventAnode::SLArEventAnode(const SLArEventAnode& right) 
  : TNamed(right) 
{
  fID = right.fID; 
  fNhits = right.fNhits;
  fIsActive = right.fIsActive; 
  fLightBacktrackerRecordSize = right.fLightBacktrackerRecordSize;
  fChargeBacktrackerRecordSize = right.fChargeBacktrackerRecordSize;
  for (const auto &mgev : right.fMegaTilesMap) {
    fMegaTilesMap.insert(
        std::make_pair(mgev.first, (SLArEventMegatile*)mgev.second->Clone())
        );
  }
}

SLArEventAnode::SLArEventAnode(SLArCfgAnode* cfg) {
  SetName(cfg->GetName()); 
  //ConfigSystem(cfg); 
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

SLArEventMegatile* SLArEventAnode::CreateEventMegatile(const int mtIdx) {
  if (fMegaTilesMap.count(mtIdx)) {
    printf("SLArEventAnode::CreateEventMegatile(%i) WARNING: Megatile nr %i already present in anode %i register\n", mtIdx, mtIdx, fID);
    return fMegaTilesMap.find(mtIdx)->second;
  }

  auto mt_event = new SLArEventMegatile(); 
  mt_event->SetIdx(mtIdx); 
  mt_event->SetLightBacktrackerRecordSize( fLightBacktrackerRecordSize); 
  mt_event->SetChargeBacktrackerRecordSize( fChargeBacktrackerRecordSize); 
  fMegaTilesMap.insert( std::make_pair(mtIdx, mt_event) );  

  return mt_event;
}

SLArEventTile* SLArEventAnode::RegisterHit(SLArEventPhotonHit* hit) {
  int mgtile_idx = hit->GetMegaTileIdx(); 
  SLArEventMegatile* mt_event = nullptr;
  if (fMegaTilesMap.count(mgtile_idx) == 0) mt_event = CreateEventMegatile(mgtile_idx);
  else mt_event = fMegaTilesMap.find(mgtile_idx)->second;

  auto t_event = mt_event->RegisterHit(hit);
  fNhits++;
  return t_event; 
  //} else {
    //printf("SLArEventAnode::RegisterHit WARNING\n"); 
    //printf("Megatile with ID %i is not in store\n", mgtile_idx); 
    //CreateEventMegatile(hit->GetMegaTileIdx());
    //return 0; 
  //}
}

int SLArEventAnode::ResetHits() {
  printf("SLArEventAnode::ResetHits() clear event on anode %i\n", fID);
  int nn = 0; 
  for (auto &mgtile : fMegaTilesMap) {
    nn += mgtile.second->ResetHits(); 
    delete mgtile.second; mgtile.second = nullptr;
  }

  fMegaTilesMap.clear();
  return nn; 
}

void SLArEventAnode::SetActive(bool is_active) {
  fIsActive = is_active; 
  for (auto &mgtile : fMegaTilesMap) {
    mgtile.second->SetActive(is_active); 
  }
}

//bool SLArEventAnode::SortHits() {
  //int isort = true;
  //for (auto &mgtile : fMegaTilesMap) {
    //isort *= mgtile.second->SortHits(); 
  //}
  //return isort;
//}
