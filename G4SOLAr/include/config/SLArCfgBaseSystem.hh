/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgBaseSystem
 * @created     : martedì lug 19, 2022 12:56:56 CEST
 */

#ifndef SLARCFGBASESYSTEM_HH

#define SLARCFGBASESYSTEM_HH


#include <iostream>
#include <fstream>
#include <map>
#include "TNamed.h"

template<class TAssemblyModule>
class SLArCfgBaseSystem : public TNamed
{
  public:
    SLArCfgBaseSystem();
    SLArCfgBaseSystem(const SLArCfgBaseSystem& cfg);
    SLArCfgBaseSystem(TString name);
    ~SLArCfgBaseSystem();

    void DumpModulesConfig();
    int  RegisterModule(TString name);
    int  RegisterModule(TAssemblyModule* array);

    TAssemblyModule* GetModuleInfo(int idx);
    TAssemblyModule* GetModule(TString name);
    std::map<TString, TAssemblyModule*>& GetModuleMap() {return fModulesMap;}

  private:
    int fNModules;
    std::map<TString, TAssemblyModule*> fModulesMap;

  public:
    ClassDefOverride(SLArCfgBaseSystem, 1);
};

#endif /* end of include guard BCPMTMAP_HH */


