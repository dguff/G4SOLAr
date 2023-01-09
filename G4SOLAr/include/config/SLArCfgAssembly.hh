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

#include "TNamed.h"
#include "TVector3.h"
#include "TH2Poly.h"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgSuperCell.hh"
#include "config/SLArCfgBaseModule.hh"

template<class TBaseModule>
class SLArCfgAssembly : public SLArCfgBaseModule {
  public: 
    enum ESubModuleReferenceFrame {kRelative = 0, kWorld = 1}; 
    SLArCfgAssembly(); 
    SLArCfgAssembly(TString name, int serie = 0); 
    SLArCfgAssembly(const SLArCfgAssembly& cfg); 
    virtual ~SLArCfgAssembly(); 

    void DumpMap(); 
    TBaseModule* FindBaseElementInMap(int ibin); 
    TBaseModule* GetBaseElement(int idx); 
    std::map<int, TBaseModule*>& GetMap() {return fElementsMap;}
    //TH2Poly* GetTH2(); 
    void RegisterElement(TBaseModule* element);
    //void SetTH2BinIdx();
    TH2Poly* BuildPolyBinHist(ESubModuleReferenceFrame kFrame = kWorld, int n = 25, int m = 25);
    TGraph* BuildGShape() override; 
    //void ResetH2Hits(); 

  protected: 
    //TH2Poly* fH2Bins; 
    int fNElements; 
    std::map<int, TBaseModule*> fElementsMap;

  public:
    ClassDefOverride(SLArCfgAssembly,1);
}; 




#endif /* end of include guard SLARCFGASSEMBLY_HH */

