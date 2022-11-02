/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventSuperCell
 * @created     : giovedì ott 20, 2022 15:40:04 CEST
 */

#include "event/SLArEventSuperCell.hh"

SLArEventSuperCell::SLArEventSuperCell()
  : TObject(), fIdx(0), fIsActive(1), fNhits(0) {}

SLArEventSuperCell::SLArEventSuperCell(int idx) 
  :TObject(), fIdx(idx), fIsActive(true), fNhits(0) {};

SLArEventSuperCell::SLArEventSuperCell(const SLArEventSuperCell& ev) 
  : TObject(ev) 
{
  fIdx = ev.fIdx; 
  fIsActive = ev.fIsActive; 
  fNhits = ev.fNhits; 
  if ( !ev.fHits.empty() ) {
    fHits.reserve(ev.fHits.size()); 
    for (const auto &hit : ev.fHits) {
      fHits.push_back((SLArEventPhotonHit*)hit->Clone()); 
    }
  }
}

SLArEventSuperCell::~SLArEventSuperCell() {
  for (auto &hit : fHits) delete hit; 
  fHits.clear(); 
  fNhits = 0;
}

int SLArEventSuperCell::RegisterHit(SLArEventPhotonHit* hit) {
  if (!hit) return -1; 
  fHits.push_back(hit); 
  fNhits++; 
  return fNhits;
}

double SLArEventSuperCell::GetTime() {
  double t = -1; 
  if (fNhits) t = fHits.front()->GetTime(); 
  return t;
}

double SLArEventSuperCell::GetTime(EPhProcess proc) {
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

bool SLArEventSuperCell::SortHits()
{
  std::sort(fHits.begin(), fHits.end(), 
            SLArEventPhotonHit::CompareHitPtrs);
            
  return true;
}

int SLArEventSuperCell::ResetHits()
{
  for (auto &hit : fHits) delete hit;
  fHits.clear(); 

  fNhits = 0;

  return fHits.size();
}

void SLArEventSuperCell::PrintHits()
{
  std::cout << "SuperCell id: " << fIdx << std::endl;
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
