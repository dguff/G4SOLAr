/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventTile
 * @created     : mercoledì ago 10, 2022 12:21:15 CEST
 */

#include "event/SLArEventTile.hh"

ClassImp(SLArEventTile)

SLArEventTile::SLArEventTile() 
  : SLArEventHitsCollection<SLArEventPhotonHit>() {}


SLArEventTile::SLArEventTile(int idx) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(idx) 
{
  fName = Form("EvTile%i", fIdx); 
}


SLArEventTile::SLArEventTile(const SLArEventTile& ev) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(ev)
{
  if (!ev.fPixelHits.empty()) {
    for (const auto &qhit : ev.fPixelHits) {
      fPixelHits.insert(
          std::make_pair(qhit.first, (SLArEventChargePixel*)qhit.second->Clone()));
    }
  }
}

SLArEventTile::~SLArEventTile() {
  for (auto &hit : fHits) delete hit;
  fHits.clear();
  fNhits = 0;
  for (auto &pixHit : fPixelHits) {
    if (pixHit.second) {
      pixHit.second->ResetHits(); 
      delete pixHit.second;
    }
  }
  fPixelHits.clear(); 
}

double SLArEventTile::GetTime() {
  double t = -1;
  if (fNhits > 0) t = fHits.at(0)->GetTime();

  return t;
}

double SLArEventTile::GetTime(EPhProcess proc) {
  double t = -1;
  if (proc == kCher)
  {
    for (auto &hit : fHits)
      {if (hit->GetProcess()==kCher) t = hit->GetTime(); break;}
  }
  else if (proc == kScnt){
    for (auto &hit : fHits) 
      {if (hit->GetProcess()==kScnt) t = hit->GetTime(); break;}
  }
  else if (proc == kAll && fHits.size() > 0)
    t = fHits.at(0)->GetTime();

  return t;
}

bool SLArEventTile::SortPixelHits()
{
  for (auto &pixHit : fPixelHits) {
    pixHit.second->SortHits(); 
  }
  return true;
}

bool SLArEventTile::SortHits() {
  std::sort(fHits.begin(), fHits.end(), SLArEventPhotonHit::CompareHitPtrs);
  SortPixelHits(); 
  return true;
}

int SLArEventTile::ResetHits()
{
  for (auto &hit : fHits) delete hit;
  fHits.clear(); 
  fNhits = 0;

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
    hit->DumpInfo(); 
  }
  if (!fPixelHits.empty()) {
    printf("Pixel readout hits:\n");
    for (const auto &pix : fPixelHits) pix.second->PrintHits(); 
  }

  printf("\n"); 
  return;
}

int SLArEventTile::RegisterChargeHit(int pixID, SLArEventChargeHit* qhit) {
  int nhit = 0; 
  if (fPixelHits.count(pixID)) {
    //printf("SLArEventTile::RegisterChargeHit(%i): pixel %i already hit.\n", pixID, pixID);
    fPixelHits[pixID]->RegisterHit(qhit); 
    nhit = fPixelHits[pixID]->GetNhits(); 
  }
  else {
    //printf("SLArEventTile::RegisterChargeHit(%i): pixel %i already hit.\n", pixID, pixID);
    SLArEventChargePixel* pixEv = new SLArEventChargePixel(pixID, qhit); 
    fPixelHits.insert(std::make_pair(pixID, pixEv));
    nhit = 1; 
  }

  //printf("SLArEventTile::RegisterChargeHit(%i): DONE.\n", pixID);
  return nhit; 
}

double SLArEventTile::GetPixelHits() {
  double nhits = 0.;
  for (const auto &pixel : fPixelHits) {
    nhits += pixel.second->GetNhits(); 
  }

  return nhits; 
}
