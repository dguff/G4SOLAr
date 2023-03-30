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
#include "TList.h"

#include "config/SLArCfgSuperCellArray.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgBaseSystem.hh"

templateClassImp(SLArCfgBaseSystem)


template<class TAssemblyModule>
SLArCfgBaseSystem<TAssemblyModule>::SLArCfgBaseSystem() 
  : SLArCfgBaseModule()//, fH2Bins(nullptr)
{
  fName = "aMapHasNoName"; 
}

template<class TAssemblyModule>
SLArCfgBaseSystem<TAssemblyModule>::SLArCfgBaseSystem(TString name) 
  : SLArCfgBaseModule()//, fH2Bins(nullptr)
{
  fName = name; 
}

template<class TAssemblyModule>
SLArCfgBaseSystem<TAssemblyModule>::SLArCfgBaseSystem(const SLArCfgBaseSystem<TAssemblyModule>& cfg) 
  : SLArCfgBaseModule(cfg) 
{
  fNElements = cfg.fNElements; 
  for (const auto &mod : cfg.fElementsMap) {
    fElementsMap.insert(
        std::make_pair(mod.first, new TAssemblyModule(*mod.second))); 
  }

  //fH2Bins = nullptr; 
  //if (cfg.fH2Bins) {
    //fH2Bins = new TH2Poly(); 

    //TList* lbin = cfg.fH2Bins->GetBins(); 
    //for (const auto& bbin : *lbin) {
      //TH2PolyBin* bin = (TH2PolyBin*)bbin; 
      //fH2Bins->AddBin(bin->GetPolygon()->Clone()); 
    //}
  //}
  
  return; 
}

template<class TAssemblyModule>
SLArCfgBaseSystem<TAssemblyModule>::~SLArCfgBaseSystem() {
  for (auto &mod : fElementsMap) {
    if (mod.second) {delete mod.second; mod.second = 0;}
  }
  fElementsMap.clear(); 
  //if (fH2Bins) {delete fH2Bins;}
}

template<class TAssemblyModule>
TAssemblyModule* SLArCfgBaseSystem<TAssemblyModule>::GetBaseElement(const char* name)
{
  TAssemblyModule* module_cfg = nullptr;
  for (const auto& mod : fElementsMap) {
    if (std::strcmp(mod.second->GetName(),name) == 0) {
      module_cfg = mod.second; 
      break;
    }
  }

  return module_cfg;
}

template<class TAssemblyModule>
TAssemblyModule* SLArCfgBaseSystem<TAssemblyModule>::GetBaseElement(int idx)
{
  return fElementsMap.find(idx)->second;
}


template<class TAssemblyModule>
void SLArCfgBaseSystem<TAssemblyModule>::RegisterElement(TAssemblyModule* mod) 
{
  int idx = mod->GetIdx();
  if (fElementsMap.count(idx)) 
  {
    std::cout<<"Base element "<<idx<<" already registered. Skip"<<std::endl;
    return;
  }

  printf("SLArCfgBaseSystem::RegisterElement(%s)...\n", mod->GetName());
  fElementsMap.insert( std::make_pair(idx, mod) );
  printf("DONE\n");
  fNElements++; 
}

template<class TAssemblyModule>
TH2Poly* SLArCfgBaseSystem<TAssemblyModule>::BuildPolyBinHist()
{
  TH2Poly* h2Bins = new TH2Poly();
  h2Bins->SetName(fName+"_bins");

  h2Bins->SetFloat();

  int iBin = 1;
  for (auto &mod : fElementsMap) 
  {
    //if (!mod.second->GetGraphShape()) {
      //mod.second->BuildGShape(); 
    //}
    auto g = mod.second->BuildGShape(); 
    TString gBinName = Form("gBin%i", iBin);
    printf("SLArCfgBaseSystem::BuildPolyBinHist: Adding bin %i\n", iBin);
    int bin_idx = h2Bins->AddBin(
        g->Clone(gBinName));
    mod.second->SetBinIdx(bin_idx);
    delete g; 
    iBin ++;
  }

  return h2Bins;
}

template<class TAssemblyModule>
void SLArCfgBaseSystem<TAssemblyModule>::DumpMap() 
{
  std::printf("SLArCfgSystem %s has %lu entries\n", 
      fName.Data(), fElementsMap.size());
  for (auto &itr : fElementsMap)
    itr.second->DumpInfo();
}


template<class TAssemblyModule>
TAssemblyModule* SLArCfgBaseSystem<TAssemblyModule>::FindBaseElementInMap(int ibin) 
{
  TAssemblyModule* module_cfg = nullptr;
  for (const auto& mod : fElementsMap) {
    if (mod.second->GetBinIdx() == ibin) {
      module_cfg = mod.second; 
      break;
    }
  }

  return module_cfg;
}

//template<class TAssemblyModule>
//void SLArCfgBaseSystem<TAssemblyModule>::ResetH2Hits() {
  //for (auto &mod : fElementsMap) {
    //if (mod.second) mod.second->ResetH2Hits(); 
  //}
//}

template class SLArCfgBaseSystem<SLArCfgSuperCellArray>;

