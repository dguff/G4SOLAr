/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgSystemPix.hh
 * @created     : giovedì nov 10, 2022 16:23:20 CET
 */

#ifndef SLARCFGSYSTEMPIX_HH

#define SLARCFGSYSTEMPIX_HH

#include <map>
#include "TH2Poly.h"
#include "config/SLArCfgBaseSystem.hh"

class SLArCfgSystemPix : public SLArCfgBaseSystem<SLArCfgMegaTile> {
  public: 
    typedef std::array<int, 3> SLArPixIdxCoord; 
    SLArCfgSystemPix(); 
    SLArCfgSystemPix(const SLArCfgBaseSystem<SLArCfgMegaTile>& cfg); 
    SLArCfgSystemPix(TString name); 

    SLArPixIdxCoord FindPixel(double x, double y); 
    void RegisterMap(size_t ilevel, TH2Poly* hmap); 
    TH2Poly* GetAnodeMap(size_t ilevel) {return fAnodeLevelsMap.find(ilevel)->second;}


  protected:
    std::map<size_t, TH2Poly*> fAnodeLevelsMap; 

  public:
    ClassDefOverride(SLArCfgSystemPix, 2); 

};





#endif /* end of include guard SLARCFGSYSTEMPIX_HH */

