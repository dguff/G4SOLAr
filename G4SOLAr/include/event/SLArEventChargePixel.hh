/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventChargePixel
 * @created     : venerdì nov 11, 2022 13:51:31 CET
 */

#ifndef SLAREVENTCHARGEPIXEL_HH

#define SLAREVENTCHARGEPIXEL_HH

#include <iostream>
#include <map>
#include "TNamed.h"

#include "event/SLArEventHitsCollection.hh"
#include "event/SLArEventChargeHit.hh"

class SLArEventChargePixel : public SLArEventHitsCollection<SLArEventChargeHit> {
  public: 
    SLArEventChargePixel(); 
    SLArEventChargePixel(const int&, const SLArEventChargeHit&); 
    SLArEventChargePixel(const SLArEventChargePixel&); 
    ~SLArEventChargePixel() {}

  private: 

  public: 
    ClassDef(SLArEventChargePixel, 1)
}; 


#endif /* end of include guard SLAREVENTCHARGEPIXEL_HH */

