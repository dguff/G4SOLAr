/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventMegatile.hh
 * @created     : mercoledì ago 10, 2022 13:42:54 CEST
 */

#ifndef SLAREVENTMEGATILE_HH

#define SLAREVENTMEGATILE_HH

#include "event/SLArEventTile.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgReadoutTile.hh"

class SLArEventMegatile : public TNamed {
  public: 
    SLArEventMegatile(); 
    SLArEventMegatile(SLArCfgMegaTile* cfg); 
    SLArEventMegatile(const SLArEventMegatile& right);
    ~SLArEventMegatile(); 

    int ConfigModule(SLArCfgMegaTile* cfg);

    std::map<int, SLArEventTile*>& GetTileMap() {return fTilesMap;}
    int GetNhits();
    int GetIdx() {return fIdx;}

    int RegisterHit(SLArEventPhotonHit* hit); 
    int ResetHits(); 

    void SetActive(bool is_active); 
    void SetIdx(int idx) {fIdx = idx;}
    bool SortHits(); 

  private:
    int fIdx; 
    bool fIsActive; 
    int fNhits; 
    std::map<int, SLArEventTile*> fTilesMap; 

  public:
    ClassDef(SLArEventMegatile, 1)
};


#endif /* end of include guard SLAREVENTMEGATILE_HH */

