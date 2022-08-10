/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgAssembly
 * @created     : martedì lug 19, 2022 11:53:34 CEST
 */

#include "TRegexp.h"
#include "TPRegexp.h"

#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgSuperCell.hh"
#include "config/SLArCfgAssembly.hh"

templateClassImp(SLArCfgAssembly)

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly() 
  : fH2Bins(nullptr), fSerie(0), fNElements(0)
{
  SetName("aAssemblyHasNoName");
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly(TString name, int serie) 
  : fH2Bins(nullptr), fNElements(0)
{
  SetName(name);
  fSerie = serie;
  printf("SLArCfgAssembly created with name %s\n", fName.Data());
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly(const SLArCfgAssembly &cfg)
  : SLArCfgBaseModule(cfg), fH2Bins(0), fNElements(0)
{
  SLArCfgAssembly<TBAseModule>();
  SetName(cfg.fName);
  fSerie = cfg.fSerie;

  for (auto &pmt : cfg.fElementsMap)
  {
    fElementsMap.insert(std::make_pair(
          pmt.first, (TBAseModule*)pmt.second->Clone()));
    fNElements++;
  }

  if (fNElements) SetTH2BinIdx();
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::~SLArCfgAssembly()
{
  if (fH2Bins) {delete fH2Bins; fH2Bins = nullptr;}
  for (auto &sc : fElementsMap)
    if (sc.second) {delete sc.second; sc.second = 0;}
  fElementsMap.clear();
  fNElements = 0;
}

template<class TBAseModule>
void SLArCfgAssembly<TBAseModule>::DumpMap() 
{
  std::printf("SLArCfgAssembly %s has %lu entries\n", 
      fName.Data(), fElementsMap.size());
  for (auto &itr : fElementsMap)
    itr.second->DumpInfo();
}

template<class TBAseModule>
void SLArCfgAssembly<TBAseModule>::RegisterElement(TBAseModule* element)
{
  int idx = element->GetIdx();
  if (fElementsMap.count(idx)) 
  {
    std::cout<<"Base element "<<idx<<" already registered. Skip"<<std::endl;
    return;
  }
  fElementsMap.insert( 
      std::make_pair(idx, element)
      );
  fNElements++; 
}

template<class TBaseModule>
TBaseModule* SLArCfgAssembly<TBaseModule>::GetBaseElement(int idx)
{
  return fElementsMap.find(idx)->second;
}

template<class TBaseModule>
void SLArCfgAssembly<TBaseModule>::BuildPolyBinHist()
{
  fH2Bins = new TH2Poly();
  fH2Bins->SetName(fName+"_bins");

  fH2Bins->SetFloat();

  int iBin = 1;
  for (auto &pmt : fElementsMap) 
  {
    TString gBinName = Form("gBin%i", iBin);
    int bin_idx = fH2Bins->AddBin(
        pmt.second->GetGraphShape()->Clone(gBinName));
    pmt.second->SetBinIdx(bin_idx);
    iBin ++;
  }
}

template<class TBaseModule>
TH2Poly* SLArCfgAssembly<TBaseModule>::GetTH2()
{
  return fH2Bins;
}

template<class TBaseModule>
void SLArCfgAssembly<TBaseModule>::SetTH2BinIdx()
{
  BuildPolyBinHist();

  for (auto &elements : fElementsMap)
  {
    double x = elements.second->GetX();
    double y = elements.second->GetY();
    int  idx = fH2Bins->FindBin(x, y);
    elements.second->SetBinIdx(idx);
  }
}

template class SLArCfgAssembly<SLArCfgSuperCell>; 
template class SLArCfgAssembly<SLArCfgReadoutTile>;
