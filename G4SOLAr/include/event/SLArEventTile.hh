/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventTile.hh
 * @created     : mercoledì ago 10, 2022 12:13:26 CEST
 */

#ifndef SLAREVENTTILE_HH

#define SLAREVENTTILE_HH

#include <iostream>
#include <vector>
#include "TObject.h"
#include "event/SLArEventPhotonHit.hh"

class SLArEventTile :  public TObject 
{
  public: 
    SLArEventTile(); 
    SLArEventTile(int idx); 
    SLArEventTile(const SLArEventTile& ev); 
    ~SLArEventTile(); 

    int GetIdx() {return fIdx;}
    int GetNhits() {return fNhits;}
    double GetTime();
    double GetTime(EPhProcess proc);
    std::vector<SLArEventPhotonHit*>& GetHits() {return fHits;}

    bool IsActive() {return fIsActive;}

    void PrintHits();

    int  RegisterHit(SLArEventPhotonHit* hit) ;
    int  ResetHits();

    bool SortHits();
    void SetActive(bool is_active) {fIsActive = is_active;}
    void SetIdx(int idx) {fIdx = idx;}


  protected:
    int fIdx; 
    bool fIsActive; 
    int fNhits; 
    std::vector<SLArEventPhotonHit*> fHits; 

    ClassDef(SLArEventTile, 1)
};

#endif /* end of include guard SLAREVENTTILE_HH */

