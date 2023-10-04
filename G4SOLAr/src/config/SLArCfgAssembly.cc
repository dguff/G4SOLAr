/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgAssembly.cc
 * @created     : Tuesday Jul 19, 2022 11:53:34 CEST
 */

#include "TRegexp.h"
#include "TPRegexp.h"
#include "TList.h"

#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgSuperCell.hh"
#include "config/SLArCfgAssembly.hh"

templateClassImp(SLArCfgAssembly)

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly() 
  : SLArCfgBaseModule()/*, fH2Bins(nullptr)*/, fNElements(0)
{
  SetName("aAssemblyHasNoName");
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly(TString name, int serie) 
  : SLArCfgBaseModule(serie)/*, fH2Bins(nullptr)*/, fNElements(0)
{
  SetName(name);
#ifdef SLAR_DEBUG
  printf("SLArCfgAssembly created with name %s\n", fName.Data());
#endif
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly(const SLArCfgAssembly &cfg)
  : SLArCfgBaseModule(cfg)/*, fH2Bins(0)*/, fNElements(0)
{
  SLArCfgAssembly<TBAseModule>();
  SetName(cfg.fName);

  for (auto &el : cfg.fElementsMap)
  {
    fElementsMap.insert(std::make_pair(
          el.first, (TBAseModule*)el.second->Clone()));
    fNElements++;
  }

  //if (fNElements) SetTH2BinIdx();
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::~SLArCfgAssembly()
{
  //if (fH2Bins) {delete fH2Bins; fH2Bins = nullptr;}
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
TH2Poly* SLArCfgAssembly<TBaseModule>::BuildPolyBinHist(
    ESubModuleReferenceFrame kFrame, 
    int n, int m)
{
  TH2Poly* h2Bins = new TH2Poly();
  
  h2Bins->SetName(fName+"_bins");

  h2Bins->SetFloat();

  int iBin = 1;
  for (auto &el : fElementsMap) 
  {
    //printf("el position (global): %g, %g, %g \n", 
        //el.second->GetPhysX(), el.second->GetPhysY(), el.second->GetPhysZ());
    TGraph* g = el.second->BuildGShape(); 
    if (kFrame == kRelative) {
      for (int i=0; i<g->GetN(); i++) {
        //printf("(%g, %g) -> ", g->GetX()[i], g->GetY()[i]);
        g->GetX()[i] -= TVector3(fPhysX, fPhysY, fPhysZ).Dot( fAxis0 ); 
        g->GetY()[i] -= TVector3(fPhysX, fPhysY, fPhysZ).Dot( fAxis1 ); 
        //printf("(%g, %g)\n", g->GetX()[i], g->GetY()[i]);
      }
      //getchar(); 
    }
    TString gBinName = Form("gBin%i", iBin);
    int bin_idx = h2Bins->AddBin(
        g->Clone(gBinName));
    el.second->SetBinIdx(bin_idx);
    iBin ++;
    delete g; 
  }

  h2Bins->ChangePartition(n, m); 
  return h2Bins;
}

/*
 *template<class TBaseModule>
 *TH2Poly* SLArCfgAssembly<TBaseModule>::GetTH2()
 *{
 *  return fH2Bins;
 *}
 */

/*
 *template<class TBaseModule>
 *void SLArCfgAssembly<TBaseModule>::SetTH2BinIdx()
 *{
 *  BuildPolyBinHist();
 *
 *  for (auto &elements : fElementsMap)
 *  {
 *    double x = elements.second->GetX();
 *    double y = elements.second->GetY();
 *    int  idx = fH2Bins->FindBin(x, y);
 *    elements.second->SetBinIdx(idx);
 *  }
 *}
 */

template<class TBaseModule>
TGraph* SLArCfgAssembly<TBaseModule>::BuildGShape() {

  double x_min =  1e10;
  double x_max = -1e10; 
  double y_min =  1e10; 
  double y_max = -1e10; 

  for (const auto &el : fElementsMap) {
    TGraph* gbin = el.second->BuildGShape(); 
    double* x = gbin->GetX(); 
    double* y = gbin->GetY(); 
    int n = gbin->GetN(); 
    double x_min_bin = *std::min_element(x, x+n);
    double x_max_bin = *std::max_element(x, x+n); 
    double y_min_bin = *std::min_element(y, y+n); 
    double y_max_bin = *std::max_element(y, y+n); 

    if (x_min_bin < x_min) x_min = x_min_bin; 
    if (x_max_bin > x_max) x_max = x_max_bin; 
    if (y_min_bin < y_min) y_min = y_min_bin; 
    if (y_max_bin > y_max) y_max = y_max_bin;

    delete gbin; 
  }

  TGraph* g = new TGraph(5); 
  g->SetPoint(0, x_min, y_min); 
  g->SetPoint(1, x_min, y_max); 
  g->SetPoint(2, x_max, y_max); 
  g->SetPoint(3, x_max, y_min); 
  g->SetPoint(4, x_min, y_min); 

  g->SetName(Form("g%s", fName.Data())); 
  return g;
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
template class SLArCfgAssembly<SLArCfgMegaTile>;
