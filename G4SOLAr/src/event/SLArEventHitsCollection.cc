/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventHitsCollection
 * @created     : Fir Nov 11, 2022 14:28:03 CET
 */

#include "event/SLArEventHitsCollection.hh"
#include "event/SLArEventChargeHit.hh"
#include "event/SLArEventPhotonHit.hh"

templateClassImp(SLArEventHitsCollection)

template<class T>
SLArEventHitsCollection<T>::SLArEventHitsCollection() 
  : TNamed(), fIdx(0), fIsActive(true), fNhits(0) {}

template<class T>
SLArEventHitsCollection<T>::SLArEventHitsCollection(int idx) 
  : TNamed(), fIdx(idx), fIsActive(true), fNhits(0) {}

template<class T>
SLArEventHitsCollection<T>::SLArEventHitsCollection(const SLArEventHitsCollection<T>& other)
  : TNamed(other) 
{
  fIdx = other.fIdx; 
  fIsActive = other.fIsActive; 
  fNhits = other.fNhits; 

  if (!other.fHits.empty()) {
    fHits.reserve(other.fHits.size()); 
    for (const auto &hit : other.fHits) {
      fHits.push_back((T*)hit->Clone());
    }
  }
}

template<class T>
SLArEventHitsCollection<T>::~SLArEventHitsCollection() {
  for (auto &hit : fHits) delete hit; 
  fHits.clear(); 
  fNhits = 0; 
}

template<class T>
int SLArEventHitsCollection<T>::RegisterHit(T* hit) {
  if (!hit) return -1; 
  fHits.push_back(hit); 
  fNhits++; 
  return fNhits;
}

template<class T>
bool SLArEventHitsCollection<T>::SortHits() {
  std::sort(fHits.begin(), fHits.end(), T::CompareHitPtrs); 
  return true; 
}

template<class T>
int SLArEventHitsCollection<T>::ResetHits() {
  for (auto &hit : fHits) delete hit; 
  fHits.clear(); 
  fNhits = 0; 
  return fHits.size(); 
}

template<class T>
void SLArEventHitsCollection<T>::PrintHits() {
  printf("Hit container ID: %i [%s]\n", fIdx, fName.Data());
  printf("- - - - - - - - - - - - - - - - - - - - - - -\n");
  for (auto &hit : fHits) {
    hit->DumpInfo(); 
  }
  printf("\n");
}

template class SLArEventHitsCollection<SLArEventPhotonHit>; 
template class SLArEventHitsCollection<SLArEventChargeHit>; 
