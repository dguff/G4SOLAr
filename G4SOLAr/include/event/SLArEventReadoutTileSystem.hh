/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventReadoutTileSystem.hh
 * @created     : mercoledì ago 10, 2022 14:29:23 CEST
 */

#ifndef SLAREVENTREADOUTTILESYSTEM_HH

#define SLAREVENTREADOUTTILESYSTEM_HH

#include "event/SLArEventMegatile.hh"
#include "config/SLArCfgSystemPix.hh"
#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgMegaTile.hh"

class SLArEventReadoutTileSystem : public TNamed {
  public:
    SLArEventReadoutTileSystem(); 
    SLArEventReadoutTileSystem(SLArCfgSystemPix* cfg);
    SLArEventReadoutTileSystem(const SLArEventReadoutTileSystem&);
    ~SLArEventReadoutTileSystem(); 

    int ConfigSystem(SLArCfgSystemPix* cfg);
    std::map<int, SLArEventMegatile*>& GetMegaTilesMap() {return fMegaTilesMap;}
    int GetNhits() {return fNhits;}
    bool IsActive() {return fIsActive;}

    int RegisterHit(SLArEventPhotonHit* hit); 
    int ResetHits(); 

    void SetActive(bool is_active); 
    bool SortHits(); 

  private:
    int fNhits; 
    bool fIsActive;
    std::map<int, SLArEventMegatile*> fMegaTilesMap;

  public:
    ClassDef(SLArEventReadoutTileSystem, 1)
};


#endif /* end of include guard SLAREVENTREADOUTTILESYSTEM_HH */

