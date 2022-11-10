/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgSystemPix
 * @created     : giovedì nov 10, 2022 16:23:20 CET
 */

#ifndef SLARCFGSYSTEMPIX_HH

#define SLARCFGSYSTEMPIX_HH

#include "config/SLArCfgBaseSystem.hh"

class SLArCfgSystemPix : public SLArCfgBaseSystem<SLArCfgMegaTile> {
  public: 
    typedef std::array<int, 3> SLArPixIdxCoord; 
    SLArCfgSystemPix(); 
    SLArCfgSystemPix(const SLArCfgBaseSystem<SLArCfgMegaTile>& cfg); 
    SLArCfgSystemPix(TString name); 

    SLArPixIdxCoord FindPixel(double x, double y); 

    ClassDefOverride(SLArCfgSystemPix, 2); 
};





#endif /* end of include guard SLARCFGSYSTEMPIX_HH */

