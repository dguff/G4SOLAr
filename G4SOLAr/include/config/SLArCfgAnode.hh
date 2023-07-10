/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgAnode.hh
 * @created     : Thursday Nov 10, 2022 16:23:20 CET
 */

#ifndef SLARCFGSYSTEMPIX_HH

#define SLARCFGSYSTEMPIX_HH

#include <map>
#include "TH2Poly.h"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgMegaTile.hh"

class SLArCfgAnode : public SLArCfgAssembly<SLArCfgMegaTile> {
  public: 
    typedef std::array<int, 3> SLArPixIdxCoord; 
    SLArCfgAnode(); 
    SLArCfgAnode(const SLArCfgAssembly<SLArCfgMegaTile>& cfg); 
    SLArCfgAnode(TString name); 
    ~SLArCfgAnode(); 

    TH2Poly* ConstructPixHistMap(const int depth, const std::vector<int>); 
    SLArPixIdxCoord FindPixel(double x, double y); 
    void RegisterMap(size_t ilevel, TH2Poly* hmap); 
    inline TH2Poly* GetAnodeMap(size_t ilevel) {return fAnodeLevelsMap.find(ilevel)->second;}
    inline int GetTPCID() const {return fTPCID;}
    inline void SetTPCID(int tpcID) {fTPCID = tpcID;}
    TVector3 GetPixelCoordinates( const SLArPixIdxCoord& coord);

  protected:
    std::map<size_t, TH2Poly*> fAnodeLevelsMap; 
    int fTPCID; 

  public:
    ClassDefOverride(SLArCfgAnode, 1); 
};





#endif /* end of include guard SLARCFGSYSTEMPIX_HH */

