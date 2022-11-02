/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventTile
 * @created     : mercoledì ago 10, 2022 12:21:15 CEST
 */

#include "event/SLArEventTile.hh"

ClassImp(SLArEventTile)

SLArEventTile::SLArEventTile() 
  : TObject(), fIdx(0), fIsActive(1), fNhits(0) {}


SLArEventTile::SLArEventTile(int idx) 
  : TObject(), fIdx(idx), fIsActive(true), fNhits(0) {}


SLArEventTile::SLArEventTile(const SLArEventTile& ev) 
  : TObject(ev)
{
  fIdx = ev.fIdx; 
  fIsActive = ev.fIsActive; 
  fNhits = ev.fNhits; 
  if (!ev.fHits.empty()) {
    fHits.reserve(ev.fHits.size()); 
    for (const auto &hit : ev.fHits) {
      fHits.push_back((SLArEventPhotonHit*)hit->Clone());
    }
  }
}

SLArEventTile::~SLArEventTile() {
  for (auto &hit : fHits) delete hit;
  fHits.clear();
  fNhits = 0;
}

int SLArEventTile::RegisterHit(SLArEventPhotonHit* hit) {
  if (!hit) return -1;
  fHits.push_back(hit);
  fNhits++;
  return fNhits;
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

bool SLArEventTile::SortHits()
{
  std::sort(fHits.begin(), fHits.end(), 
            SLArEventPhotonHit::CompareHitPtrs);
            
  return true;
}

int SLArEventTile::ResetHits()
{
  for (auto &hit : fHits) delete hit;
  fHits.clear(); 

  fNhits = 0;

  return fHits.size();
}

void SLArEventTile::PrintHits()
{
  std::cout << "Hit container id: " << fIdx << std::endl;
  std::cout << "-----------------------------------------"
            << std::endl;
  int nhits = 0;
  for (auto &hit : fHits)
  {
    std::cout << "hit " << nhits << " at " << hit->GetTime() 
              << " ns " << std::endl;
    nhits++;
  }

  std::cout << "\n" << std::endl;
  
}
