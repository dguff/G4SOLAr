/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgAssembly.cc
 * @created     : Tuesday Jul 19, 2022 11:53:34 CEST
 */

#include "TRegexp.h"
//#include "TPRegexp.h"
//#include "TList.h"

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
  fElementsMap.reserve(50); 
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::SLArCfgAssembly(TString name, int serie) 
  : SLArCfgBaseModule(serie)/*, fH2Bins(nullptr)*/, fNElements(0)
{
  SetName(name);
  fElementsMap.reserve(50);
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

  const size_t n_elements = cfg.fElementsMap.size();
  fElementsMap.reserve(n_elements); 
  for (size_t i_element = 0; i_element < n_elements; i_element++)
  {
    fElementsMap.push_back( TBAseModule(cfg.fElementsMap.at(i_element)) ); 
    fNElements++;
  }

  //if (fNElements) SetTH2BinIdx();
}

template<class TBAseModule>
SLArCfgAssembly<TBAseModule>::~SLArCfgAssembly()
{
  //if (fH2Bins) {delete fH2Bins; fH2Bins = nullptr;}
  //for (auto &sc : fElementsMap)
    //if (sc) {delete sc.second; sc.second = 0;}
  fElementsMap.clear();
  fNElements = 0;
}

template<class TBAseModule>
void SLArCfgAssembly<TBAseModule>::DumpMap() const
{
  const size_t n_elements = fElementsMap.size(); 
  std::printf("SLArCfgAssembly %s has %lu entries\n", 
      fName.Data(), n_elements);
  for (size_t i_element = 0; i_element < n_elements; i_element++) 
  {
    const auto& itr = fElementsMap.at(i_element); 
    itr.DumpInfo();
  }
}

template<class TBAseModule>
void SLArCfgAssembly<TBAseModule>::RegisterElement(TBAseModule& element)
{
  int id = element.GetID();
  if (fIDtoIdxMap.count(id)) 
  {
    std::cout<<"Base element with id "<<id<<" already registered. Skip"<<std::endl;
    return;
  }
  element.SetIdx( fElementsMap.size() ); 
  fIDtoIdxMap.insert( std::make_pair( id, element.GetIdx() ) ); 
  fElementsMap.push_back(element);
  fNElements++; 
}

template<class TBaseModule>
TBaseModule& SLArCfgAssembly<TBaseModule>::GetBaseElement(int idx)
{
  return fElementsMap.at(idx); 
  //auto it = fElementsMap.find(idx);
  //if (it != fElementsMap.end()) {
    //return it->second;
  //}
  //else {
    //return nullptr;
  //}
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
  const size_t n_elements = fElementsMap.size(); 
  for (size_t i_element = 0; i_element < n_elements; i_element++) {
    auto& el = GetBaseElement(i_element); 
    //printf("el position (global): %g, %g, %g \n", 
        //el.second->GetPhysX(), el.second->GetPhysY(), el.second->GetPhysZ());
    TGraph g = el.BuildGShape(); 
    if (kFrame == kRelative) {
      for (int i=0; i<g.GetN(); i++) {
        //printf("(%g, %g) -> ", g->GetX()[i], g->GetY()[i]);
        g.GetX()[i] -= TVector3(fPhysX, fPhysY, fPhysZ).Dot( fAxis0 ); 
        g.GetY()[i] -= TVector3(fPhysX, fPhysY, fPhysZ).Dot( fAxis1 ); 
        //printf("(%g, %g)\n", g->GetX()[i], g->GetY()[i]);
      }
      //getchar(); 
    }
    TString gBinName = Form("gBin%i", iBin);
    int bin_idx = h2Bins->AddBin((TGraph*)g.Clone(gBinName));
    el.SetBinIdx(bin_idx);
    fBinToIdxMap.insert( std::make_pair(bin_idx, i_element) ); 
    iBin ++;
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
TGraph SLArCfgAssembly<TBaseModule>::BuildGShape() {

  double x_min =  1e10;
  double x_max = -1e10; 
  double y_min =  1e10; 
  double y_max = -1e10; 

  const size_t n_elements = fElementsMap.size();

  for (size_t i_element = 0; i_element < n_elements; i_element++) {
    auto& el = GetBaseElement(i_element); 
    TGraph gbin = el.BuildGShape(); 
    double* x = gbin.GetX(); 
    double* y = gbin.GetY(); 
    int n = gbin.GetN(); 
    double x_min_bin = *std::min_element(x, x+n);
    double x_max_bin = *std::max_element(x, x+n); 
    double y_min_bin = *std::min_element(y, y+n); 
    double y_max_bin = *std::max_element(y, y+n); 

    if (x_min_bin < x_min) x_min = x_min_bin; 
    if (x_max_bin > x_max) x_max = x_max_bin; 
    if (y_min_bin < y_min) y_min = y_min_bin; 
    if (y_max_bin > y_max) y_max = y_max_bin;
  }

  TGraph g(5); 
  g.SetPoint(0, x_min, y_min); 
  g.SetPoint(1, x_min, y_max); 
  g.SetPoint(2, x_max, y_max); 
  g.SetPoint(3, x_max, y_min); 
  g.SetPoint(4, x_min, y_min); 

  g.SetName(Form("g%s", fName.Data())); 
  return g;
}

template<class TBaseModule>
TBaseModule& SLArCfgAssembly<TBaseModule>::GetBaseElementByBin(int ibin) 
{
  //TBaseModule* module_cfg = nullptr;
  //if (fBinToIdxMap.find(ibin) == fBinToIdxMap.end()) {
    //for (const auto& mod : fElementsMap) {
      //if (mod.second->GetBinIdx() == ibin) {
        //module_cfg = mod.second; 
        //break;
      //}
    //}
  //}
  //else {
    int module_idx = fBinToIdxMap.find(ibin)->second;
    TBaseModule& module_cfg = fElementsMap.at(module_idx); 
  //}
  
  return module_cfg;
}

template<class TBaseModule>
TBaseModule& SLArCfgAssembly<TBaseModule>::GetBaseElementByID(int id) 
{
  int module_idx = fIDtoIdxMap.find(id)->second;
  TBaseModule& module_cfg = fElementsMap.at(module_idx); 

  return module_cfg;
}

template class SLArCfgAssembly<SLArCfgSuperCell>; 
template class SLArCfgAssembly<SLArCfgReadoutTile>;
template class SLArCfgAssembly<SLArCfgMegaTile>;
