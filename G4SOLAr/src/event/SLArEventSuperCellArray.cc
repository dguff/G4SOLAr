/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArEventSuperCellArray.cc
 * @created     Thur Oct 20, 2022 16:16:03 CEST
 */

#include "event/SLArEventSuperCellArray.hh"

templateClassImp(SLArEventSuperCellArray)

template class SLArEventSuperCellArray<SLArEventSuperCell*>;
template class SLArEventSuperCellArray<std::unique_ptr<SLArEventSuperCell>>;

template<class S>
SLArEventSuperCellArray<S>::SLArEventSuperCellArray()
  : TNamed(), fNhits(0), fIsActive(true) {}

template<>
SLArEventSuperCellArrayPtr::SLArEventSuperCellArray(const SLArEventSuperCellArray& ev)
  : TNamed(ev) 
{
  fNhits = ev.fNhits; 
  fIsActive = ev.fIsActive; 
  for (const auto &sc : ev.fSuperCellMap) {
    fSuperCellMap.insert(
        std::make_pair(sc.first, new SLArEventSuperCell(*sc.second) ) );
  }
  return;
}

template<>
SLArEventSuperCellArrayUniquePtr::SLArEventSuperCellArray(const SLArEventSuperCellArray& ev)
  : TNamed(ev) 
{
  fNhits = ev.fNhits; 
  fIsActive = ev.fIsActive; 
  for (const auto &sc : ev.fSuperCellMap) {
    fSuperCellMap.insert(
        std::make_pair(sc.first, std::make_unique<SLArEventSuperCell>(*sc.second) ) );
  }
  return;
}

template<class S>
SLArEventSuperCellArray<S>::SLArEventSuperCellArray(SLArCfgSuperCellArray* cfg) 
  : SLArEventSuperCellArray()
{
  SetName(cfg->GetName());
  //ConfigSystem(cfg); 
  return;
}

template<>
SLArEventSuperCellArrayPtr::~SLArEventSuperCellArray() {
  for (auto &scevent : fSuperCellMap) {
    scevent.second->ResetHits();
    delete scevent.second;
  }
  fSuperCellMap.clear(); 
}

template<>
SLArEventSuperCellArrayUniquePtr::~SLArEventSuperCellArray() {
  for (auto &scevent : fSuperCellMap) {
    scevent.second->ResetHits();
  }
  fSuperCellMap.clear(); 
}

template<>
int SLArEventSuperCellArrayPtr::ConfigSystem(SLArCfgSuperCellArray* cfg) {
  int nsc = 0; 
  for (const auto &sc : cfg->GetMap()) {
      if (fSuperCellMap.count(sc.first) == 0) {
        fSuperCellMap.insert(
              std::make_pair(sc.first, new SLArEventSuperCell(sc.first))
            ); 
        nsc++;
    }
  }

  return nsc; 
}

template<>
int SLArEventSuperCellArrayUniquePtr::ConfigSystem(SLArCfgSuperCellArray* cfg) {
  int nsc = 0; 
  for (const auto &sc : cfg->GetMap()) {
      if (fSuperCellMap.count(sc.first) == 0) {
        fSuperCellMap.insert(
              std::make_pair(sc.first,std::make_unique<SLArEventSuperCell>(sc.first))
            ); 
        nsc++;
    }
  }

  return nsc; 
}

template<>
SLArEventSuperCell*& SLArEventSuperCellArrayPtr::GetOrCreateEventSuperCell(const int scIdx) {
  auto it = fSuperCellMap.find(scIdx); 

  if (it != fSuperCellMap.end()) {
    //printf("SLArEventAnode::CreateEventMegatile(%i) WARNING: Megatile nr %i already present in SuperCell Array %s register\n", scIdx, scIdx, fName.Data());
    return fSuperCellMap.find(scIdx)->second;
  }
  else {
    SLArEventSuperCell* sc_event = new SLArEventSuperCell(scIdx); 
    sc_event->SetBacktrackerRecordSize( fLightBacktrackerRecordSize ); 
    fSuperCellMap.insert( std::make_pair(scIdx, std::move(sc_event)) );

    return fSuperCellMap[scIdx];
  }
}

template<>
std::unique_ptr<SLArEventSuperCell>& SLArEventSuperCellArrayUniquePtr::GetOrCreateEventSuperCell(const int scIdx) {
  auto it = fSuperCellMap.find(scIdx); 

  if (it != fSuperCellMap.end()) {
    //printf("SLArEventAnode::CreateEventMegatile(%i) WARNING: Megatile nr %i already present in SuperCell Array %s register\n", scIdx, scIdx, fName.Data());
    return fSuperCellMap.find(scIdx)->second;
  }
  else {
    std::unique_ptr<SLArEventSuperCell> sc_event = std::make_unique<SLArEventSuperCell>(scIdx); 
    sc_event->SetBacktrackerRecordSize( fLightBacktrackerRecordSize ); 
    fSuperCellMap.insert( std::make_pair(scIdx, std::move(sc_event)) );

    return fSuperCellMap[scIdx];
  }
}

template<class S>
S& SLArEventSuperCellArray<S>::RegisterHit(const SLArEventPhotonHit& hit) {
  int sc_idx = hit.GetTileIdx(); 
  auto& sc_event = GetOrCreateEventSuperCell(sc_idx);
  sc_event->RegisterHit(hit); 

  fNhits++;
  return fSuperCellMap[sc_idx];

  //} else {
    //printf("SLArEventSuperCellArray::RegisterHit WARNING\n"); 
    //printf("SuperCell with ID %i is not in store [%i,%i,%i]\n", 
        //sc_idx, hit->GetMegaTileIdx(), hit->GetRowTileNr(), hit->GetTileNr()); 
    //return 0; 
  //}
}

template<>
int SLArEventSuperCellArrayPtr::ResetHits() {
  int nn = 0; 
  for (auto &sc : fSuperCellMap) {
    nn += sc.second->ResetHits(); 
    delete sc.second;
  }
  fSuperCellMap.clear();
  fNhits = 0; 
  return nn; 
}

template<>
int SLArEventSuperCellArrayUniquePtr::ResetHits() {
  int nn = 0; 
  for (auto &sc : fSuperCellMap) {
    nn += sc.second->ResetHits(); 
  }
  fSuperCellMap.clear();
  fNhits = 0; 
  return nn; 
}

template<class S>
int SLArEventSuperCellArray<S>::SoftResetHits() {
  int nn = 0; 
  for (auto &sc : fSuperCellMap) {
    nn += sc.second->ResetHits(); 
  }
  fSuperCellMap.clear();
  fNhits = 0; 
  return nn; 
}


template<class S>
void SLArEventSuperCellArray<S>::SetActive(bool is_active) {
  fIsActive = is_active; 
  for (auto &sc : fSuperCellMap) {
    sc.second->SetActive(is_active); 
  }
}

//bool SLArEventSuperCellArray::SortHits() {
  //int isort = true;
  //for (auto &sc : fSuperCellMap) {
    //isort *= sc.second->SortHits(); 
  //}
  //return isort;
//}













