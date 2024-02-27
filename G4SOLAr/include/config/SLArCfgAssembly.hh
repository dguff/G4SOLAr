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
    TBaseModule& GetBaseElementByBin(int ibin); 
    TBaseModule& GetBaseElementByID(int ibin); 
    TBaseModule& GetBaseElement(int idx); 
    inline std::vector<TBaseModule>& GetMap() {return fElementsMap;}
    inline const std::vector<TBaseModule>& GetConstMap() const {return fElementsMap;}
    void RegisterElement(TBaseModule& element);
    virtual TH2Poly* BuildPolyBinHist(ESubModuleReferenceFrame kFrame = kWorld, int n = 25, int m = 25);
    TGraph BuildGShape() override; 

  protected: 
    int fNElements; 
    std::vector<TBaseModule> fElementsMap;
    std::map<int, int> fBinToIdxMap;
    std::map<int, int> fIDtoIdxMap; 

  public:
    ClassDefOverride(SLArCfgAssembly,3);
}; 


#endif /* end of include guard SLARCFGASSEMBLY_HH */

