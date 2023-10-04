/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventChargeHit.hh
 * @created     : Fri Nov 11, 2022 13:35:51 CET
 */

#ifndef SLAREVENTCHARGEHIT_HH

#define SLAREVENTCHARGEHIT_HH

#include "event/SLArEventGenericHit.hh"

class SLArEventChargeHit : public SLArEventGenericHit {
  public: 
    SLArEventChargeHit(); 
    SLArEventChargeHit(float time, int trkId, int primaryID); 
    SLArEventChargeHit(const SLArEventChargeHit& h);
    ~SLArEventChargeHit() {}; 

    void   DumpInfo(); 

  private:
    int    fTrkID;
    int    fPrimaryID; 

  public: 
    ClassDef(SLArEventChargeHit, 1);
};

#endif /* end of include guard SLAREVENTCHARGEHIT_HH */

