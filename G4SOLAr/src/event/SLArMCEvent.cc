/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArMCEvent
 * @created     : mercoledì ago 10, 2022 11:52:40 CEST
 */

#include "event/SLArMCEvent.hh"
#include "TRandom3.h"
ClassImp(SLArMCEvent)

SLArMCEvent::SLArMCEvent() : 
  fEvNumber(0), fDirection{0, 0, 0}
{}

SLArMCEvent::~SLArMCEvent()
{
  std::cerr << "Deleting SLArMCEvent..." << std::endl;
  for (auto &evAnode : fEvAnode) {
    if (evAnode.second) delete evAnode.second;
  }
  fEvAnode.clear(); 

  for ( auto &scArray : fEvSuperCellArray ) {
    if (scArray.second) delete scArray.second;
  }
  fEvSuperCellArray.clear(); 

  for (auto &p : fSLArPrimary) {
    delete p; p = nullptr; 
  }
  fSLArPrimary.clear();
  std::cerr << "~SLArMCEvent DONE" << std::endl;
}

int SLArMCEvent::ConfigAnode(std::map<int, SLArCfgAnode*> anodeCfg)
{
  for (const auto& anode : anodeCfg) {
    SLArEventAnode* evAnode = new SLArEventAnode(anode.second); 
    evAnode->SetID(anode.second->GetIdx()); 
    //evAnode->ConfigSystem(anode.second); 
    fEvAnode.insert(std::make_pair(anode.first, evAnode)); 
  }
  
  return fEvAnode.size();
}

int SLArMCEvent::ConfigSuperCellSystem(SLArCfgSystemSuperCell* supercellSysCfg)
{
  for (const auto& scArray : supercellSysCfg->GetMap()) {
    if (fEvSuperCellArray.count(scArray.first)) {
      printf("SLArMCEvent::ConfigSuperCellSystem() WARNING: "); 
      printf("SuperCelll array with index %i is aleady stored in the MCEvent. Skipping.\n", scArray.first);
      continue;
    }

    SLArEventSuperCellArray* evSCArray = new SLArEventSuperCellArray(scArray.second); 
    evSCArray->ConfigSystem(scArray.second); 
    fEvSuperCellArray.insert(std::make_pair(scArray.first, evSCArray));
  }

  return fEvSuperCellArray.size();
}

SLArEventAnode* SLArMCEvent::GetEventAnodeByID(int id) {
  for (auto &anode : fEvAnode) {
    if (anode.second->GetID() == id) {return anode.second;}
  }

  return nullptr;
}

int SLArMCEvent::SetEvNumber(int nEv)
{
  fEvNumber = nEv;
  return fEvNumber;
}

void SLArMCEvent::Reset()
{
  for (auto &anode : fEvAnode) {
    if (anode.second) anode.second->ResetHits();
  }

  for (auto &scArray : fEvSuperCellArray) {
    if (scArray.second) scArray.second->ResetHits(); 
  }

  for (auto &p : fSLArPrimary) {
    delete p; p = nullptr; 
  }
  fSLArPrimary.clear(); 
  fDirection = {0, 0, 1};
  fEvNumber = -1;
}

void SLArMCEvent::SetDirection(double* dir) {
  if (dir) {
    fDirection.at(0) = dir[0];  
    fDirection.at(1) = dir[1];  
    fDirection.at(2) = dir[2];  
  } 
}

void SLArMCEvent::SetDirection(double px, double py, double pz) {
    fDirection.at(0) = px;  
    fDirection.at(1) = py;  
    fDirection.at(2) = pz;  
}

bool SLArMCEvent::CheckIfPrimary(int trkId) {
  bool is_primary = false; 
  for (const auto &p : fSLArPrimary) {
    if (trkId == p->GetTrackID()) {
      is_primary = true; 
      break;
    }
  }
  return is_primary; 
}
