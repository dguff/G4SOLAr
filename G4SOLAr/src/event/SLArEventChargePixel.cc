/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventChargePixel
 * @created     : venerdì nov 11, 2022 14:58:07 CET
 */

#include "event/SLArEventChargePixel.hh"

ClassImp(SLArEventChargePixel)

SLArEventChargePixel::SLArEventChargePixel() 
  : SLArEventHitsCollection<SLArEventChargeHit>()
{}

SLArEventChargePixel::SLArEventChargePixel(const int idx, SLArEventChargeHit* hit)
  : SLArEventHitsCollection<SLArEventChargeHit>(idx) 
{
  fName = Form("EvPix%i", fIdx); 
  RegisterHit(hit); 
}


