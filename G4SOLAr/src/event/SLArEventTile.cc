/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventTile
 * @created     : mercoledì ago 10, 2022 12:21:15 CEST
 */

#include "event/SLArEventTile.hh"

ClassImp(SLArEventTile)

SLArEventTile::SLArEventTile() 
  : SLArEventHitsCollection<SLArEventPhotonHit>(), fChargeBacktrackerRecordSize(0) {}


SLArEventTile::SLArEventTile(const int idx) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(idx), fChargeBacktrackerRecordSize(0) 
{
  fName = Form("EvTile%i", fIdx); 
}


SLArEventTile::SLArEventTile(const SLArEventTile& ev) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(ev), fChargeBacktrackerRecordSize(0)
{
  fChargeBacktrackerRecordSize = ev.fChargeBacktrackerRecordSize;
  if (!ev.fPixelHits.empty()) {
    for (const auto &qhit : ev.fPixelHits) {
      fPixelHits.insert(
          std::make_pair(qhit.first, (SLArEventChargePixel*)qhit.second->Clone()));
    }
  }
}

SLArEventTile::~SLArEventTile() {
  ResetHits();
}

double SLArEventTile::GetTime() const {
  double t = -1;
  if (fNhits > 0) t = fHits.begin()->first * fClockUnit;

  return t;
}

double SLArEventTile::GetTime(EPhProcess proc) const {
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

int SLArEventTile::ResetHits()
{
  SLArEventHitsCollection::ResetHits();

  for (auto &pix : fPixelHits) {
    if (pix.second) {
      pix.second->ResetHits(); 
      delete pix.second;
    }
  }
  fPixelHits.clear(); 

  return fHits.size();
}

void SLArEventTile::PrintHits()
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

SLArEventChargePixel* SLArEventTile::RegisterChargeHit(const int pixID, const SLArEventChargeHit* qhit) {
  SLArEventChargePixel* pixEv = nullptr;
  if (fPixelHits.count(pixID)) {
    //printf("SLArEventTile::RegisterChargeHit(%i): pixel %i already hit.\n", pixID, pixID);
    pixEv = fPixelHits[pixID];
    pixEv->RegisterHit(qhit); 
  }
  else {
    //printf("SLArEventTile::RegisterChargeHit(%i): pixel %i already hit.\n", pixID, pixID);
    pixEv = new SLArEventChargePixel(pixID, qhit); 
    pixEv->SetBacktrackerRecordSize( fChargeBacktrackerRecordSize ); 
    fPixelHits.insert(std::make_pair(pixID, pixEv));
  }

  //printf("SLArEventTile::RegisterChargeHit(%i): DONE.\n", pixID);
  return pixEv; 
}

double SLArEventTile::GetPixelHits() const {
  double nhits = 0.;
  for (const auto &pixel : fPixelHits) {
    nhits += pixel.second->GetNhits(); 
  }

  return nhits; 
}
