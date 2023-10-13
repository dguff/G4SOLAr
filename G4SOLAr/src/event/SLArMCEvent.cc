/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArMCEvent
 * @created     : mercoledì ago 10, 2022 11:52:40 CEST
 */

#include "SLArAnalysisManager.hh"
#include "event/SLArMCEvent.hh"
#include "TRandom3.h"
#include <cstdio>
#include <typeinfo>
templateClassImp(SLArMCEvent)

template class SLArMCEvent<SLArMCPrimaryInfoPtr*, SLArEventAnodePtr*, SLArEventSuperCellArrayPtr*>;
template class SLArMCEvent<std::unique_ptr<SLArMCPrimaryInfoUniquePtr>, std::unique_ptr<SLArEventAnodeUniquePtr>, std::unique_ptr<SLArEventSuperCellArrayUniquePtr>>;

template<class P, class A, class X>
SLArMCEvent<P,A,X>::SLArMCEvent() : 
  fEvNumber(0), fDirection{0, 0, 0}
{
   fSLArPrimary.reserve(50);
}

template<>
SLArMCEventPtr::SLArMCEvent(const SLArMCEventPtr& ev) : TObject(ev)
{
  fEvNumber = ev.fEvNumber;
  fDirection = ev.fDirection;

  for (const auto& p : ev.fSLArPrimary) {
    fSLArPrimary.push_back( new SLArMCPrimaryInfoPtr(*p) );
  }

  for (const auto& itr : ev.fEvAnode) {
    fEvAnode[itr.first] = new SLArEventAnodePtr(*itr.second);
  }

  for (const auto & itr : ev.fEvSuperCellArray) {
    fEvSuperCellArray[itr.first] = new SLArEventSuperCellArrayPtr(*itr.second);
  }
}

template<>
SLArMCEventUniquePtr::SLArMCEvent(const SLArMCEventUniquePtr& ev) : TObject(ev)
{
  fEvNumber = ev.fEvNumber;
  fDirection = ev.fDirection;

  for (const auto& p : ev.fSLArPrimary) {
    fSLArPrimary.push_back( std::make_unique<SLArMCPrimaryInfoUniquePtr>(*p) );
  }

  for (const auto& itr : ev.fEvAnode) {
    fEvAnode[itr.first] = std::make_unique<SLArEventAnodeUniquePtr>(*itr.second);
  }

  for (const auto & itr : ev.fEvSuperCellArray) {
    fEvSuperCellArray[itr.first] = std::make_unique<SLArEventSuperCellArrayUniquePtr>(*itr.second);
  }
}



template<>
SLArMCEventPtr::~SLArMCEvent()
{
  std::cerr << "Deleting SLArMCEvent..." << std::endl;
  for (auto &evAnode : fEvAnode) {
    evAnode.second->ResetHits();
    delete evAnode.second;
  }
  fEvAnode.clear(); 

  for ( auto &scArray : fEvSuperCellArray ) {
    scArray.second->ResetHits();
    delete scArray.second;
  }
  fEvSuperCellArray.clear(); 

  for (auto &primary : fSLArPrimary) {
    delete primary;
  }
  fSLArPrimary.clear();
  std::cerr << "~SLArMCEvent DONE" << std::endl;
}

template<>
SLArMCEventUniquePtr::~SLArMCEvent()
{
  std::cerr << "Deleting SLArMCEvent..." << std::endl;
  for (auto &evAnode : fEvAnode) {
    evAnode.second.get()->ResetHits();
  }
  fEvAnode.clear(); 

  for ( auto &scArray : fEvSuperCellArray ) {
    scArray.second.get()->ResetHits();
  }
  fEvSuperCellArray.clear(); 

  fSLArPrimary.clear();
  std::cerr << "~SLArMCEvent DONE" << std::endl;
}

template<>
int SLArMCEventUniquePtr::ConfigAnode(std::map<int, SLArCfgAnode*> anodeCfg)
{
  for (const auto& anode : anodeCfg) {
    fEvAnode.insert(std::make_pair(anode.first, std::make_unique<SLArEventAnodeUniquePtr>(anode.second)));
    fEvAnode[anode.first]->SetID(anode.second->GetIdx());
  }
  std::cout << "fEvAnode type is " << typeid(fEvAnode).name() << std::endl;

  getchar();  
  return fEvAnode.size();
}

template<>
int SLArMCEventPtr::ConfigAnode(std::map<int, SLArCfgAnode*> anodeCfg)
{
  for (const auto& anode : anodeCfg) {
    fEvAnode.insert(std::make_pair(anode.first, new SLArEventAnodePtr(anode.second)));
    fEvAnode[anode.first]->SetID(anode.second->GetIdx());
    //evAnode->ConfigSystem(anode.second); 
  }

  return fEvAnode.size();
}

template<>
int SLArMCEventUniquePtr::ConfigSuperCellSystem(SLArCfgSystemSuperCell* supercellSysCfg)
{
  for (const auto& scArray : supercellSysCfg->GetMap()) {
    if (fEvSuperCellArray.count(scArray.first)) {
      printf("SLArMCEvent::ConfigSuperCellSystem() WARNING: "); 
      printf("SuperCelll array with index %i is aleady stored in the MCEvent. Skipping.\n", scArray.first);
      continue;
    }

    fEvSuperCellArray.insert(std::make_pair(scArray.first, std::make_unique<SLArEventSuperCellArrayUniquePtr>(scArray.second)));
    fEvSuperCellArray[scArray.first]->ConfigSystem(scArray.second);
  }

  return fEvSuperCellArray.size();
}

template<>
int SLArMCEventPtr::ConfigSuperCellSystem(SLArCfgSystemSuperCell* supercellSysCfg)
{
  for (const auto& scArray : supercellSysCfg->GetMap()) {
    if (fEvSuperCellArray.count(scArray.first)) {
      printf("SLArMCEvent::ConfigSuperCellSystem() WARNING: "); 
      printf("SuperCelll array with index %i is aleady stored in the MCEvent. Skipping.\n", scArray.first);
      continue;
    }

    fEvSuperCellArray.insert(std::make_pair(scArray.first, new SLArEventSuperCellArrayPtr(scArray.second)));
    fEvSuperCellArray[scArray.first]->ConfigSystem(scArray.second);
  }

  return fEvSuperCellArray.size();
}

template<>
std::unique_ptr<SLArEventAnodeUniquePtr>& SLArMCEventUniquePtr::GetEventAnodeByID(const int& id) {
  for (auto &anode : fEvAnode) {
    if (anode.second->GetID() == id) {return anode.second;}
  }

  throw 4;
}

template<>
SLArEventAnodePtr*& SLArMCEventPtr::GetEventAnodeByID(const int& id) {
  for (auto &anode : fEvAnode) {
    if (anode.second->GetID() == id) {return anode.second;}
  }

  throw 4;
}


template<class P, class A, class X>
int SLArMCEvent<P,A,X>::SetEvNumber(int nEv)
{
  fEvNumber = nEv;
  return fEvNumber;
}

template<>
void SLArMCEventUniquePtr::Reset()
{
  for (auto &anode : fEvAnode) {
    anode.second->ResetHits();
  }

  for (auto &scArray : fEvSuperCellArray) {
    scArray.second->ResetHits(); 
  }

  fSLArPrimary.clear(); 

  fDirection = {0, 0, 1};
  fEvNumber = -1;
}

template<>
void SLArMCEventPtr::Reset()
{
  for (auto &anode : fEvAnode) {
    anode.second->ResetHits();
  }

  for (auto &scArray : fEvSuperCellArray) {
    scArray.second->ResetHits(); 
  }

  for (auto &p : fSLArPrimary) {
    delete p; 
  }
  fSLArPrimary.clear(); 

  fDirection = {0, 0, 1};
  fEvNumber = -1;
}

template<class P, class A, class X>
void SLArMCEvent<P,A,X>::SetDirection(double* dir) {
  if (dir) {
    fDirection.at(0) = dir[0];  
    fDirection.at(1) = dir[1];  
    fDirection.at(2) = dir[2];  
  } 
}

template<class P, class A, class X>
void SLArMCEvent<P,A,X>::SetDirection(double px, double py, double pz) {
    fDirection.at(0) = px;  
    fDirection.at(1) = py;  
    fDirection.at(2) = pz;  
}

template<class P, class A, class X>
bool SLArMCEvent<P,A,X>::CheckIfPrimary(int trkId) const {
  bool is_primary = false; 
  for (const auto &p : fSLArPrimary) {
    if (trkId == p->GetTrackID()) {
      is_primary = true; 
      break;
    }
  }
  return is_primary; 
}

template<>
size_t SLArMCEventUniquePtr::RegisterPrimary(std::unique_ptr<SLArMCPrimaryInfoUniquePtr> p) {
  fSLArPrimary.push_back( std::move(p) );
  return fSLArPrimary.size();
}

template<>
size_t SLArMCEventPtr::RegisterPrimary(SLArMCPrimaryInfoPtr* p) {
  fSLArPrimary.push_back( std::move(p) );
  return fSLArPrimary.size();
}

