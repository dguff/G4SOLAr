/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMCEvent
 * @created     : giovedì feb 13, 2020 12:17:11 CET
 */

#include "event/SLArMCEvent.hh"
ClassImp(SLArMCEvent)

SLArMCEvent::SLArMCEvent() : 
  fEvNumber(0), fSLArPrimary(nullptr)
{
  //fSystemPMT  = new SLArEventSystemPMT();
  //fSystemHodo = new SLArEventSystemHodo();
  fSLArPrimary  = new SLArMCPrimaryInfo();
}

SLArMCEvent::~SLArMCEvent()
{
  std::cerr << "Deleting SLArMCEvent..." << std::endl;
  //if (fSystemPMT)  delete fSystemPMT; 
  //if (fSystemHodo) delete fSystemPMT; 
  if (fSLArPrimary)  delete fSLArPrimary;
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
  if (fSLArPrimary ) fSLArPrimary ->ResetParticle();
  fEvNumber = -1;
}
