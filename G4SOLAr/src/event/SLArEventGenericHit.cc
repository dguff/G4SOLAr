/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventGenericHit.cc
 * @created     : Fri nov 11, 2022 14:07:12 CET
 */

#include "event/SLArEventGenericHit.hh"

ClassImp(SLArEventGenericHit)

SLArEventGenericHit::SLArEventGenericHit()
  : fTime(-1), fProducerTrkID(-1), fPrimaryProducerTrkID(-1) {}

SLArEventGenericHit::SLArEventGenericHit(const SLArEventGenericHit& other) 
  : TObject(other) 
{
  fTime = other.fTime; 
  fProducerTrkID = other.fProducerTrkID;
  fPrimaryProducerTrkID = other.fPrimaryProducerTrkID;
}

bool SLArEventGenericHit::operator<(const SLArEventGenericHit& other) const {
  return fTime < other.fTime; 
}

bool SLArEventGenericHit::CompareHitPtrs(const SLArEventGenericHit* left, const SLArEventGenericHit* right) {
  return left->fTime < right->fTime; 
}
