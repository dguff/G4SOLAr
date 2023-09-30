/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventSuperCell
 * @created     : giovedì ott 20, 2022 15:40:04 CEST
 */

#include "event/SLArEventSuperCell.hh"

ClassImp(SLArEventSuperCell)

SLArEventSuperCell::SLArEventSuperCell() 
  : SLArEventHitsCollection<SLArEventPhotonHit>() {}

SLArEventSuperCell::SLArEventSuperCell(int idx) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(idx) {}

SLArEventSuperCell::SLArEventSuperCell(const SLArEventSuperCell& ev) 
  : SLArEventHitsCollection<SLArEventPhotonHit>(ev) {}

SLArEventSuperCell::~SLArEventSuperCell() {
  ResetHits();
}

double SLArEventSuperCell::GetTime() const {
  double t = -1; 
  if (fNhits) t = fHits.begin()->first * fClockUnit;
  return t;
}

double SLArEventSuperCell::GetTime(EPhProcess proc) const {
  double t = -1; 
  printf("TO BE IMPLEMENTED\n");
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

void SLArEventSuperCell::PrintHits()
{
  std::cout << "SuperCell id: " << fIdx << std::endl;
  std::cout << "-----------------------------------------"
            << std::endl;
  int nhits = 0;
  for (auto &hit : fHits)
  {
    std::cout << "hit " << nhits << " at " << hit.first * fClockUnit 
              << " ns " << std::endl;
    nhits++;
  }

  std::cout << "\n" << std::endl;
  
}
