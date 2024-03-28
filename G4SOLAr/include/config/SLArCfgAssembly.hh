/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgAssembly.hh
 * @created     : Tuesday Jul 19, 2022 11:12:12 CEST
 */

#ifndef SLARCFGASSEMBLY_HH

#define SLARCFGASSEMBLY_HH

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

//#include "TNamed.h"
//#include "TVector3.h"
#include "TH2Poly.h"
//#include "config/SLArCfgReadoutTile.hh"
//#include "config/SLArCfgSuperCell.hh"
#include <config/SLArCfgBaseModule.hh>

template<class TBaseModule>
class SLArCfgAssembly : public SLArCfgBaseModule {
  public: 
    enum ESubModuleReferenceFrame {kRelative = 0, kWorld = 1}; 
    SLArCfgAssembly(); 
    SLArCfgAssembly(TString name, int serie = 0); 
    SLArCfgAssembly(const SLArCfgAssembly& cfg); 
    virtual ~SLArCfgAssembly(); 

    virtual void DumpMap() const;
    virtual void DumpInfo() const override; 
    inline TBaseModule& GetBaseElementByBin(const int ibin) {
      int module_idx = fBinToIdxMap.find(ibin)->second;
      return fElementsMap.at(module_idx); 
    }
    inline const TBaseModule& GettBaseElementByBin(const int ibin) const {
      int module_idx = fBinToIdxMap.find(ibin)->second;
      return fElementsMap.at(module_idx); 
    }; 
    inline TBaseModule& GetBaseElementByID(int const id) {
      int module_idx = fIDtoIdxMap.find(id)->second;
      return fElementsMap.at(module_idx); 
    }
    inline const TBaseModule& GetBaseElementByID(const int id) const {
      int module_idx = fIDtoIdxMap.find(id)->second;
      return fElementsMap.at(module_idx); 
    }; 
    inline TBaseModule& GetBaseElement(int const idx) {
      return fElementsMap.at(idx); 
    }
    inline const TBaseModule& GetBaseElement(const int idx) const {
      return fElementsMap.at(idx); 
    };
    inline std::vector<TBaseModule>& GetMap() {return fElementsMap;}
    inline const std::vector<TBaseModule>& GetConstMap() const {return fElementsMap;}
    void RegisterElement(TBaseModule& element);
    virtual TH2Poly* BuildPolyBinHist(ESubModuleReferenceFrame kFrame = kWorld, int n = 25, int m = 25);
    TGraph BuildGShape() const override; 

  protected: 
    int fNElements; 
    std::vector<TBaseModule> fElementsMap;
    std::map<int, int> fBinToIdxMap;
    std::map<int, int> fIDtoIdxMap; 

  public:
    ClassDefOverride(SLArCfgAssembly,3);
}; 


#endif /* end of include guard SLARCFGASSEMBLY_HH */

