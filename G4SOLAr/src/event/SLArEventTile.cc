/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventTile
 * @created     : mercoledì ago 10, 2022 12:21:15 CEST
 */

#include <memory>
#include "event/SLArEventTile.hh"

templateClassImp(SLArEventTile)

template class SLArEventTile<SLArEventChargePixel*>; 
template class SLArEventTile<std::unique_ptr<SLArEventChargePixel>>;

template<class P>
SLArEventTile<P>::SLArEventTile() 
  : SLArEventHitsCollection<SLArEventPhotonHit>(), fChargeBacktrackerRecordSize(0) 
{}


template<class P>
SLArEventTile<P>::SLArEventTile(const int idx) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(idx), fChargeBacktrackerRecordSize(0) 
{
  fName = Form("EvTile%i", fIdx); 
}


template<>
SLArEventTileUniquePtr::SLArEventTile(const SLArEventTile& ev) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(ev), fChargeBacktrackerRecordSize(0)
{
  fChargeBacktrackerRecordSize = ev.fChargeBacktrackerRecordSize;
  if (!ev.fPixelHits.empty()) {
    for (const auto &qhit : ev.fPixelHits) {
      fPixelHits[qhit.first] = std::make_unique<SLArEventChargePixel>(*qhit.second);
    }
  }
}

template<>
SLArEventTilePtr::SLArEventTile(const SLArEventTile& ev) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(ev), fChargeBacktrackerRecordSize(0)
{
  fChargeBacktrackerRecordSize = ev.fChargeBacktrackerRecordSize;
  if (!ev.fPixelHits.empty()) {
    for (const auto &qhit : ev.fPixelHits) {
      fPixelHits[qhit.first] = new SLArEventChargePixel(*qhit.second);
    }
  }
}

template<>
int SLArEventTileUniquePtr::ResetHits()
{
  SLArEventHitsCollection::ResetHits();

  for (auto &pix : fPixelHits) {
      pix.second->ResetHits(); 
  }
  fPixelHits.clear(); 

  return fHits.size();
}

template<>
int SLArEventTilePtr::ResetHits()
{
  SLArEventHitsCollection::ResetHits();

  for (auto &pix : fPixelHits) {
    pix.second->ResetHits(); 
    delete pix.second;
  }
  fPixelHits.clear(); 

  return fHits.size();
}

template<class P> 
int SLArEventTile<P>::SoftResetHits() 
{
  SLArEventHitsCollection::ResetHits();

  for (auto &pix : fPixelHits) {
      pix.second->ResetHits(); 
  }
  fPixelHits.clear(); 

  return fHits.size();
}

template<class P>
SLArEventTile<P>::~SLArEventTile() {
  ResetHits();
}

template<class P>
double SLArEventTile<P>::GetTime() const {
  double t = -1;
  if (fNhits > 0) t = fHits.begin()->first * fClockUnit;

  return t;
}

template<class P>
double SLArEventTile<P>::GetTime(EPhProcess proc) const {
  double t = -1;
  printf("TO BE FIXED\n");
  //if (proc == kCher)
  //{
    //for (auto &hit : fHits)
      //{if (hit->GetProcess()==kCher) t = hit->GetTime(); break;}
  //}
  //else if (proc == kScnt){
    //for (auto &hit : fHits) 
      //{if (hit->GetProcess()==kScnt) t = hit->GetTime(); break;}
  //}
  //else if (proc == kAll && fHits.size() > 0)
    //t = fHits.at(0)->GetTime();

  return t;
}

//bool SLArEventTile::SortPixelHits()
//{
  //for (auto &pixHit : fPixelHits) {
    //pixHit.second->SortHits(); 
  //}
  //return true;
//}

//bool SLArEventTile::SortHits() {
  //std::sort(fHits.begin(), fHits.end(), SLArEventPhotonHit::CompareHitPtrs);
  //SortPixelHits(); 
  //return true;
//}


template<class P>
void SLArEventTile<P>::PrintHits() const
{
  printf("*********************************************\n");
  printf("Hit container ID: %i [%s]\n", fIdx, fName.Data());
  printf("*********************************************\n");
  for (auto &hit : fHits) {
    SLArEventHitsCollection::Dump();
  }
  if (!fPixelHits.empty()) {
    printf("Pixel readout hits:\n");
    for (const auto &pix : fPixelHits) pix.second->PrintHits(); 
  }

  printf("\n"); 
  return;
}

template<>
std::unique_ptr<SLArEventChargePixel>& SLArEventTileUniquePtr::RegisterChargeHit(const int& pixID, const SLArEventChargeHit& qhit) {
  
  auto it = fPixelHits.find(pixID);

  if (it != fPixelHits.end()) {
    //printf("SLArEventTile::RegisterChargeHit(%i): pixel %i already hit.\n", pixID, pixID);
    it->second->RegisterHit(qhit); 
    return it->second;
  }
  else {
    //printf("SLArEventTile::RegisterChargeHit(%i): creating new pixel hit collection.\n", pixID);
    std::unique_ptr<SLArEventChargePixel> pixEv = std::make_unique<SLArEventChargePixel>(pixID, qhit); 
    pixEv->SetBacktrackerRecordSize( fChargeBacktrackerRecordSize ); 
    fPixelHits.insert(std::make_pair(pixID, std::move(pixEv)));
    return fPixelHits[pixID];
  }

}

template<>
SLArEventChargePixel*& SLArEventTilePtr::RegisterChargeHit(const int& pixID, const SLArEventChargeHit& qhit) {
  
  auto it = fPixelHits.find(pixID);

  if (it != fPixelHits.end()) {
    //printf("SLArEventTile::RegisterChargeHit(%i): pixel %i already hit.\n", pixID, pixID);
    it->second->RegisterHit(qhit); 
    return it->second;
  }
  else {
    //printf("SLArEventTile::RegisterChargeHit(%i): creating new pixel hit collection.\n", pixID);
    SLArEventChargePixel* pixEv = new SLArEventChargePixel(pixID, qhit); 
    pixEv->SetBacktrackerRecordSize( fChargeBacktrackerRecordSize ); 
    fPixelHits.insert(std::make_pair(pixID, std::move(pixEv)));
    return fPixelHits[pixID];
  }

}

template<class P>
double SLArEventTile<P>::GetPixelHits() const {
  double nhits = 0.;
  for (const auto &pixel : fPixelHits) {
    nhits += pixel.second->GetNhits(); 
  }

  return nhits; 
}
