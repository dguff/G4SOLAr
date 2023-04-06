/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventSuperCellArray.hh
 * @created     : Thur Oct 20, 2022 16:08:33 CEST
 */

#ifndef SLAREVENTSUPERCELLARRAY_HH

#define SLAREVENTSUPERCELLARRAY_HH

#include "event/SLArEventSuperCell.hh"
#include "config/SLArCfgSuperCellArray.hh"

class SLArEventSuperCellArray : public TNamed {
  public: 
    SLArEventSuperCellArray(); 
    SLArEventSuperCellArray(SLArCfgSuperCellArray* cfg); 
    SLArEventSuperCellArray(const SLArEventSuperCellArray&); 
    ~SLArEventSuperCellArray();

    int ConfigSystem(SLArCfgSuperCellArray* cfg); 
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
    ClassDef(SLArEventSuperCellArray, 1); 
}; 


#endif /* end of include guard SLAREVENTSUPERCELLARRAY */

