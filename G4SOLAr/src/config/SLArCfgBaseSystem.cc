/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgBaseSystem.cc
 * @created     : martedì lug 19, 2022 13:05:36 CEST
 */

#include <map>
#include "TObjString.h"
#include "TObjArray.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include "TH2Poly.h"

#include "config/SLArCfgSuperCellArray.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgBaseSystem.hh"

templateClassImp(SLArCfgBaseSystem)

template<class TAssemblyModule>
SLArCfgBaseSystem<TAssemblyModule>::SLArCfgBaseSystem() : fNModules(0)
{
  fName = "aMapHasNoName"; 
}

template<class TAssemblyModule>
SLArCfgBaseSystem<TAssemblyModule>::SLArCfgBaseSystem(TString name) : fNModules(0)
{
  fName = name;
}

template<class TAssemblyModule>
SLArCfgBaseSystem<TAssemblyModule>::SLArCfgBaseSystem(const SLArCfgBaseSystem &cfg)
  : TNamed(cfg)
{
  for (auto array : cfg.fModulesMap)
  {
    fModulesMap.insert(
        std::make_pair(array.first, 
          (TAssemblyModule*)array.second->Clone())
        );
    fNModules += array.second->GetMap().size();
  }
}

template<class TAssemblyModule>
SLArCfgBaseSystem<TAssemblyModule>::~SLArCfgBaseSystem()
{
  std::cerr << "Deleting SLArCfgBaseSystem..." << std::endl;

  for (auto &itr : fModulesMap)
    if (itr.second) {delete itr.second; itr.second = 0;}
  fModulesMap.clear();
  std::cerr << "SLArCfgBaseSystem DONE" << std::endl;
}

template<class TAssemblyModule>
void SLArCfgBaseSystem<TAssemblyModule>::DumpModulesConfig() 
{
  std::printf("SLArCfgBaseSystem %s has %i PMTs\n", 
      fName.Data(), fNModules);
  for (auto &itr : fModulesMap){
    itr.second->DumpMap();
    printf("\n");
  }
}

template<class TAssemblyModule>
int SLArCfgBaseSystem<TAssemblyModule>::RegisterModule(TString name)
{
  TAssemblyModule* array = new TAssemblyModule(name);

  if (fModulesMap.count(name))
  {
    printf("SLArCfgBaseSystem::RegisterArray(%s): ", name.Data());
    printf("Array already present. Clear previous version.\n");
    TAssemblyModule* old = fModulesMap.find(name)->second;
    fNModules -= old->GetMap().size();
    delete old; old = 0;
    fModulesMap.erase(fModulesMap.find(name));
  }
  
  fModulesMap.insert(std::make_pair(name, array));
  
  fNModules += array->GetMap().size();  
  return fNModules;
}

template<class TAssemblyModule>
int SLArCfgBaseSystem<TAssemblyModule>::RegisterModule(TAssemblyModule* array)
{
  if (fModulesMap.count(array->GetName()))
  {
    printf("SLArCfgBaseSystem::RegisterArray(%s): ", array->GetName());
    printf("Array already present. Clear previous version.\n");
    TAssemblyModule* old = fModulesMap.find(array->GetName())->second;
    fNModules -= old->GetMap().size();
    delete old; old = 0;
    fModulesMap.erase(fModulesMap.find(array->GetName()));
  }

  fModulesMap.insert(std::make_pair(array->GetName(), array));

  fNModules += array->GetMap().size();  
  return fNModules;
}


template<class TAssemblyModule>
TAssemblyModule* SLArCfgBaseSystem<TAssemblyModule>::GetModule(TString name)
{
  TAssemblyModule* array = nullptr;
  if (fModulesMap.count(name))
  {
    array = fModulesMap.find(name)->second;
  }
  return array;
}

template<class TAssemblyModule>
TAssemblyModule* SLArCfgBaseSystem<TAssemblyModule>::GetModule(int idx)
{
  TAssemblyModule* module_cfg = nullptr;
  for (const auto& mod : fModulesMap) {
    if (mod.second->GetIdx() == idx) {
      module_cfg = mod.second; 
      break;
    }
  }

  return module_cfg;
}

template class SLArCfgBaseSystem<SLArCfgSuperCellArray>;
template class SLArCfgBaseSystem<SLArCfgMegaTile>;
