/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventSuperCell
 * @created     : giovedì ott 20, 2022 15:28:53 CEST
 */

#ifndef SLAREVENTSUPERCELL_HH

#define SLAREVENTSUPERCELL_HH

#include <iostream>
#include <vector>
#include "TObject.h"
#include "event/SLArEventPhotonHit.hh"

class SLArEventSuperCell : public TObject {
  public: 
    SLArEventSuperCell(); 
    SLArEventSuperCell(int idx); 
    SLArEventSuperCell(const SLArEventSuperCell& ev); 
    ~SLArEventSuperCell(); 

    int GetIdx() {return fIdx;}
    int GetNhits() {return fNhits;}
    double GetTime(); 
    double GetTime(EPhProcess proc); 
    std::vector<SLArEventPhotonHit*>& GetHits() {return fHits;}

    bool IsActive() {return fIsActive;}

    void PrintHits(); 

    int RegisterHit(SLArEventPhotonHit* hit); 
    int ResetHits(); 

    bool SortHits(); 
    void SetActive(bool is_active) {fIsActive = is_active;}
    void SetIdx(int idx) {fIdx = idx;}

  protected:
    int fIdx; 
    bool fIsActive; 
    int fNhits; 
    std::vector<SLArEventPhotonHit*> fHits;

  public:
    ClassDef(SLArEventSuperCell, 1); 
}; 


#endif /* end of include guard SLAREVENTSUPERCELL_HH */

