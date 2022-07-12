/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMCEvent
 * @created     : giovedì feb 13, 2020 12:17:11 CET
 */

#include "event/SLArMCEvent.hh"
#include "TRandom3.h"
ClassImp(SLArMCEvent)

SLArMCEvent::SLArMCEvent() : 
  fEvNumber(0)
{
  //fSystemPMT  = new SLArEventSystemPMT();
  //fSystemHodo = new SLArEventSystemHodo();
}

SLArMCEvent::~SLArMCEvent()
{
  std::cerr << "Deleting SLArMCEvent..." << std::endl;
  //if (fSystemPMT)  delete fSystemPMT; 
  //if (fSystemHodo) delete fSystemPMT; 
  for (auto &p : fSLArPrimary) {
    delete p; p = nullptr; 
  }
  fSLArPrimary.clear();
  std::cerr << "SLArMCEvent DONE" << std::endl;
}

//int SLArMCEvent::ConfigPMTSystem(SLArSystemConfigPMT* pmtSysCfg)
//{
  //if (!fSystemPMT) {
    //std::cout << "SLArMCEvent::ConfigPMTSystem: fSystemPMT is null!"
              //<< std::endl;
    //return 0;
  //}

  //fSystemPMT->Config(pmtSysCfg);
  //return fSystemPMT->GetNPMTs();
//}

//int SLArMCEvent::ConfigHodoSystem(SLArSystemConfigHodo* hodoSysCfg)
//{
  //if (!fSystemHodo) {
    //std::cout << "SLArMCEvent::ConfigHodoSystem: fSystemHodo is null!"
              //<< std::endl;
    //return 0;
  //}

  //fSystemHodo->Config(hodoSysCfg);

  //return fSystemHodo->GetModuleMap().size();
//}

int SLArMCEvent::SetEvNumber(int nEv)
{
  fEvNumber = nEv;
  return fEvNumber;
}

void SLArMCEvent::Reset()
{
  //if (fSystemPMT ) fSystemPMT ->ResetHits();
  //if (fSystemHodo) fSystemHodo->Reset();
  for (auto &p : fSLArPrimary) {
    //printf("deleting primary...\n"); 
    //p->PrintParticle(); 
    delete p; p = nullptr; 
  }
  fSLArPrimary.clear(); 
  fDirection = {0, 0, 1};
  fEvNumber = -1;
}

void SLArMCEvent::SetDirection(double* dir) {
  if (dir) {
    fDirection.at(0) = dir[0];  
    fDirection.at(1) = dir[1];  
    fDirection.at(2) = dir[2];  
  } else {
    double cosTheta = 2*gRandom->Rndm() - 1.;
    double phi = TMath::TwoPi()*gRandom->Rndm();
    double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
    double ux = sinTheta*std::cos(phi),
             uy = sinTheta*std::sin(phi),
             uz = cosTheta;

    fDirection.at(0) = ux;  
    fDirection.at(1) = uy;  
    fDirection.at(2) = uz;   
  }
}

void SLArMCEvent::SetDirection(double px, double py, double pz) {
    fDirection.at(0) = px;  
    fDirection.at(1) = py;  
    fDirection.at(2) = pz;  
}



bool SLArMCEvent::CheckIfPrimary(int trkId) {
  bool is_primary = false; 
  for (const auto &p : fSLArPrimary) {
    if (trkId == p->GetTrackID()) {
      is_primary = true; 
      break;
    }
  }
  return is_primary; 
}
