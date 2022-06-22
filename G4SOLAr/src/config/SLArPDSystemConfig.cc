/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArPDSystemConfig
 * @created     : lunedì feb 10, 2020 17:55:26 CET
 */

#include <map>
#include "TObjString.h"
#include "TObjArray.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include "TH2Poly.h"

#include "config/SLArPDSystemConfig.hh"
//#include "event/BCEventHitPMT.hh"

ClassImp(SLArPDSystemConfig)

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// Implementation of SLArPDSystemConfig methods
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SLArPDSystemConfig::SLArPDSystemConfig() : fNSuperCells(0)
{
  fName = "aMapHasNoName"; 
}

SLArPDSystemConfig::SLArPDSystemConfig(TString name) : fNSuperCells(0)
{
  fName = name;
}

SLArPDSystemConfig::SLArPDSystemConfig(const SLArPDSystemConfig &cfg)
  : TNamed(cfg)
{
  for (auto array : cfg.fArrayMap)
  {
    fArrayMap.insert(
        std::make_pair(array.first, 
          (SLArCfgSuperCellArray*)array.second->Clone())
        );
    fNSuperCells += array.second->GetMap().size();
  }
}

SLArPDSystemConfig::~SLArPDSystemConfig()
{
  std::cerr << "Deleting SLArPDSystemConfig..." << std::endl;

  for (auto &itr : fArrayMap)
    if (itr.second) {delete itr.second; itr.second = 0;}
  fArrayMap.clear();
  std::cerr << "SLArPDSystemConfig DONE" << std::endl;
}

void SLArPDSystemConfig::DumpPMTSysConfig() 
{
  std::printf("SLArPDSystemConfig %s has %i PMTs\n", 
      fName.Data(), fNSuperCells);
  for (auto &itr : fArrayMap){
    itr.second->DumpMap();
    printf("\n");
  }
}

int SLArPDSystemConfig::RegisterArray(TString name)
{
  SLArCfgSuperCellArray* array = new SLArCfgSuperCellArray(name);

  if (fArrayMap.count(name))
  {
    printf("SLArPDSystemConfig::RegisterArray(%s): ", name.Data());
    printf("Array already present. Clear previous version.\n");
    SLArCfgSuperCellArray* old = fArrayMap.find(name)->second;
    fNSuperCells -= old->GetMap().size();
    delete old; old = 0;
    fArrayMap.erase(fArrayMap.find(name));
  }
  
  fArrayMap.insert(std::make_pair(name, array));
  
  fNSuperCells += array->GetMap().size();  
  return fNSuperCells;
}

/*
 *int SLArPDSystemConfig::RegisterArray(TString name, TString ascii_path)
 *{
 *  SLArCfgSuperCellArray* array = new SLArCfgSuperCellArray(name);
 *  array->LoadPMTMapAscii(ascii_path, name);
 *
 *  if (fArrayMap.count(name))
 *  {
 *    printf("SLArPDSystemConfig::RegisterArray(%s): ", name.Data());
 *    printf("Array already present. Clear previous version.\n");
 *    SLArCfgSuperCellArray* old = fArrayMap.find(name)->second;
 *    fNSuperCells -= old->GetMap().size();
 *    delete old; old = 0;
 *    fArrayMap.erase(fArrayMap.find(name));
 *  }
 *  
 *  fArrayMap.insert(std::make_pair(name, array));
 *  
 *  fNSuperCells += array->GetMap().size();  
 *  return fNSuperCells;
 *}
 *
 */
/*
 *int SLArPDSystemConfig::RegisterArray(TString name, 
 *                                     TString ascii_path, 
 *                                     double rot)
 *{
 *  SLArCfgSuperCellArray* array = new SLArCfgSuperCellArray(name);
 *  array->LoadPMTMapAscii(ascii_path, name, rot);
 *
 *  if (fArrayMap.count(name))
 *  {
 *    printf("SLArPDSystemConfig::RegisterArray(%s): ", name.Data());
 *    printf("Array already present. Clear previous version.\n");
 *    SLArCfgSuperCellArray* old = fArrayMap.find(name)->second;
 *    fNSuperCells -= old->GetMap().size();
 *    delete old; old = 0;
 *    fArrayMap.erase(fArrayMap.find(name));
 *  }
 *  
 *  fArrayMap.insert(std::make_pair(name, array));
 *  
 *  fNSuperCells += array->GetMap().size();  
 *  return fNSuperCells;
 *}
 *
 */
int SLArPDSystemConfig::RegisterArray(SLArCfgSuperCellArray* array)
{
  if (fArrayMap.count(array->GetName()))
  {
    printf("SLArPDSystemConfig::RegisterArray(%s): ", array->GetName());
    printf("Array already present. Clear previous version.\n");
    SLArCfgSuperCellArray* old = fArrayMap.find(array->GetName())->second;
    fNSuperCells -= old->GetMap().size();
    delete old; old = 0;
    fArrayMap.erase(fArrayMap.find(array->GetName()));
  }

  fArrayMap.insert(std::make_pair(array->GetName(), array));

  fNSuperCells += array->GetMap().size();  
  return fNSuperCells;
}

SLArCfgSuperCell* SLArPDSystemConfig::GetSuperCellInfo(int idx)
{
  SLArCfgSuperCell* cfg = 0;
  for (auto array : fArrayMap)
  {
    if (array.second->GetMap().count(idx)){
      cfg = array.second->GetMap().find(idx)->second;
      break;
    }
  }

  if (!cfg) std::cout << "SLArPDSystemConfig::GetPMTinfo(%i) "
    << "unable to recover PMT configuration\n";
  return cfg;
}

//void SLArPDSystemConfig::ClearPMTHits()
//{
  //for (auto &array : fArrayMap)
    //array.second->ClearSuperCellArrayHits();
//}

void SLArPDSystemConfig::ResetConfig()
{
  std::cout << "SLArPDSystemConfig::ResetConfig" << std::endl;
  fNSuperCells = 0;

  std::cout << "Clear fArrayMap" << std::endl;
  for (auto &array : fArrayMap)
  {
    if (array.second) {
      array.second->ResetConfig();
      delete array.second; 
      array.second = 0;
    }
  }
  fArrayMap.clear();
}

SLArCfgSuperCellArray* SLArPDSystemConfig::GetArray(TString name)
{
  SLArCfgSuperCellArray* array = nullptr;
  if (fArrayMap.count(name))
  {
    array = fArrayMap.find(name)->second;
  }
  return array;
}
