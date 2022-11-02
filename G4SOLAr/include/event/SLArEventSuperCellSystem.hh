/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventSuperCellSystem
 * @created     : giovedì ott 20, 2022 16:08:33 CEST
 */

#ifndef SLAREVENTSUPERCELLSYSTEM_HH

#define SLAREVENTSUPERCELLSYSTEM_HH

#include "event/SLArEventSuperCell.hh"
#include "config/SLArCfgSuperCellArray.hh"
#include "config/SLArCfgBaseSystem.hh"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArCfgSCSys; 

class SLArEventSuperCellSystem : public TNamed {
  public: 
    SLArEventSuperCellSystem(); 
    SLArEventSuperCellSystem(SLArCfgSCSys* cfg); 
    SLArEventSuperCellSystem(const SLArEventSuperCellSystem&); 
    ~SLArEventSuperCellSystem();

    int ConfigSystem(SLArCfgSCSys* cfg); 
    std::map<int, SLArEventSuperCell*>& GetSuperCellMap() {return fSuperCellMap;}
    int GetNhits() {return fNhits;}
    bool IsActive() {return fIsActive;}

    int RegisterHit(SLArEventPhotonHit* hit); 
    int ResetHits(); 

    void SetActive(bool is_active); 
    bool SortHits(); 

  private: 
    int fNhits; 
    bool fIsActive; 
    std::map<int, SLArEventSuperCell*> fSuperCellMap;

  public:
    ClassDef(SLArEventSuperCellSystem, 1); 
}; 


#endif /* end of include guard SLAREVENTSUPERCELLSYSTEM_HH */

