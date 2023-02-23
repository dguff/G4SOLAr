/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventGenericHit
 * @created     : venerdì nov 11, 2022 13:58:33 CET
 */

#ifndef SLAREVENTGENERICHIT_HH

#define SLAREVENTGENERICHIT_HH

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <algorithm>

class SLArEventGenericHit : public TObject {
  public: 
    SLArEventGenericHit(); 
    SLArEventGenericHit(const SLArEventGenericHit& h); 
    virtual ~SLArEventGenericHit() {}

    virtual void DumpInfo() {} 

    virtual float GetTime() {return fTime;}
    virtual void  SetTime(float t) {fTime = t;}

    virtual bool operator< (const SLArEventGenericHit& other) const; 
    static  bool CompareHitPtrs(const SLArEventGenericHit* left, const SLArEventGenericHit* right); 

  protected:
    float fTime; 
  
  public: 
    ClassDef(SLArEventGenericHit, 1)
}; 

inline bool SLArEventGenericHit::operator<(const SLArEventGenericHit& other) const {
  return fTime < other.fTime; 
}

inline bool SLArEventGenericHit::CompareHitPtrs(const SLArEventGenericHit* left, const SLArEventGenericHit* right) {
  return left->fTime < right->fTime; 
}
#endif /* end of include guard SLAREVENTGENERICHIT_HH */

