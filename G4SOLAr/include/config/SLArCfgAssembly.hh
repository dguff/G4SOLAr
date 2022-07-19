/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgAssembly
 * @created     : martedì lug 19, 2022 11:12:12 CEST
 */

#ifndef SLARCFGASSEMBLY_HH

#define SLARCFGASSEMBLY_HH

#include <iostream>
#include <fstream>
#include <map>

#include "TNamed.h"
#include "TVector3.h"
#include "TH2Poly.h"
#include "config/SLArCfgBaseModule.hh"

template<class TBaseModule>
class SLArCfgAssembly : public SLArCfgBaseModule {
  public: 
    SLArCfgAssembly(); 
    SLArCfgAssembly(TString name, int serie = 0); 
    SLArCfgAssembly(const SLArCfgAssembly& cfg); 
    virtual ~SLArCfgAssembly(); 

    void DumpMap(); 
    int GetAssemblySeries() {return fSerie;}
    TBaseModule* GetBaseElement(int idx); 
    std::map<int, TBaseModule*>& GetMap() {return fElementsMap;}
    TH2Poly* GetTH2(); 
    void RegisterElement(TBaseModule* element);
    void SetTH2BinIdx();

  protected: 
    TH2Poly* fH2Bins; 
    int fSerie;
    int fNElements; 
    std::map<int, TBaseModule*> fElementsMap;
    
    // private methods
    virtual void BuildPolyBinHist();

  public:
    ClassDef(SLArCfgAssembly,1);
}; 




#endif /* end of include guard SLARCFGASSEMBLY_HH */

