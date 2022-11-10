/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgAssembly.cc
 * @created     : Tuesday Jul 19, 2022 11:53:34 CEST
 */

#include "TRegexp.h"
#include "TPRegexp.h"

#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgSuperCell.hh"
#include "config/SLArCfgAssembly.hh"

templateClassImp(SLArCfgAssembly)

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly() 
  : SLArCfgBaseModule(), fH2Bins(nullptr), fNElements(0)
{
  SetName("aAssemblyHasNoName");
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly(TString name, int serie) 
  : SLArCfgBaseModule(serie), fH2Bins(nullptr), fNElements(0)
{
  SetName(name);
  printf("SLArCfgAssembly created with name %s\n", fName.Data());
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly(const SLArCfgAssembly &cfg)
  : SLArCfgBaseModule(cfg), fH2Bins(0), fNElements(0)
{
  SLArCfgAssembly<TBAseModule>();
  SetName(cfg.fName);

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
    if (!pmt.second->GetGraphShape()) {
      pmt.second->BuildGShape(); 
    }
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

template<class TBaseModule>
void SLArCfgAssembly<TBaseModule>::BuildGShape() {
  if (!fH2Bins) BuildPolyBinHist(); 

  double x_min = fH2Bins->GetXaxis()->GetXmin(); 
  double x_max = fH2Bins->GetXaxis()->GetXmax(); 
  double y_min = fH2Bins->GetYaxis()->GetXmin(); 
  double y_max = fH2Bins->GetYaxis()->GetXmax(); 

  fGShape = new TGraph(5); 
  fGShape->SetPoint(0, x_min, y_min); 
  fGShape->SetPoint(1, x_min, y_max); 
  fGShape->SetPoint(2, x_max, y_max); 
  fGShape->SetPoint(3, x_max, y_min); 
  fGShape->SetPoint(4, x_min, y_min); 

  fGShape->SetName(Form("g%s", fName.Data())); 
  return;
}

template<class TBaseModule>
TBaseModule* SLArCfgAssembly<TBaseModule>::FindBaseElementInMap(int ibin) 
{
  TBaseModule* module_cfg = nullptr;
  for (const auto& mod : fElementsMap) {
    if (mod.second->GetBinIdx() == ibin) {
      module_cfg = mod.second; 
      break;
    }
  }

  return module_cfg;
}



template class SLArCfgAssembly<SLArCfgSuperCell>; 
template class SLArCfgAssembly<SLArCfgReadoutTile>;
