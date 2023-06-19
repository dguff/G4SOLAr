/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventGenericHit.cc
 * @created     : Fri nov 11, 2022 14:07:12 CET
 */

#include "event/SLArEventGenericHit.hh"

ClassImp(SLArEventGenericHit)

SLArEventGenericHit::SLArEventGenericHit()
  : fTime(-1) {}

SLArEventGenericHit::SLArEventGenericHit(const SLArEventGenericHit& other) 
  : TObject(other) 
{
  fTime = other.fTime; 
}
