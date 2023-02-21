/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventChargeHit.cc
 * @created     : Fri Nov 11, 2022 13:44:52 CET
 */

#include "event/SLArEventChargeHit.hh"

ClassImp(SLArEventChargeHit)

SLArEventChargeHit::SLArEventChargeHit()
  : SLArEventGenericHit(), fTrkID(-1), fPrimaryID(-1) {}

SLArEventChargeHit::SLArEventChargeHit(float time, int trkId, int primaryID) 
  : SLArEventGenericHit(), fTrkID(trkId), fPrimaryID(primaryID) {
  fTime = time; 
}

SLArEventChargeHit::SLArEventChargeHit(const SLArEventChargeHit& h)
  : SLArEventGenericHit(h) 
{
  fTrkID = h.fTrkID; 
  fPrimaryID = h.fPrimaryID;
}

void SLArEventChargeHit::DumpInfo() {
  printf("charge hit at t = %g, trk id: %i, primaryID: %i\n", 
      fTime, fTrkID, fPrimaryID);
}

