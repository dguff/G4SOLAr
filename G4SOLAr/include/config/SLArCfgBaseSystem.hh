/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgBaseSystem.hh
 * @created     : martedì lug 19, 2022 12:56:56 CEST
 */

#ifndef SLARCFGBASESYSTEM_HH

#define SLARCFGBASESYSTEM_HH


#include <iostream>
#include <fstream>
#include <map>
#include "SLArCfgMegaTile.hh"
#include "SLArCfgBaseModule.hh"
#include "SLArCfgSuperCellArray.hh"
#include "TH2Poly.h"


template<class TAssemblyModule>
class SLArCfgBaseSystem : public SLArCfgBaseModule
{
  public:
    SLArCfgBaseSystem();
    SLArCfgBaseSystem(const SLArCfgBaseSystem& cfg);
    SLArCfgBaseSystem(TString name);
    ~SLArCfgBaseSystem();

    void BuildPolyBinHist(); 
    void DumpMap();
    TAssemblyModule* GetBaseElement(int idx); 
    TAssemblyModule* GetBaseElement(const char* name);
    TAssemblyModule* FindBaseElementInMap(int ibin); 
    std::map<int, TAssemblyModule*>& GetMap() {return fElementsMap;}
    TH2Poly* GetTH2() {return fH2Bins;}
    void SetTH2BinIdx();
    void RegisterElement(TAssemblyModule* mod); 

  protected:
    TH2Poly* fH2Bins; 
    int fNElements; 
    std::map<int, TAssemblyModule*> fElementsMap;

  public:
    ClassDefOverride(SLArCfgBaseSystem, 2);
};

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArCfgSystemSuperCell; 

#endif /* end of include guard BCPMTMAP_HH */


