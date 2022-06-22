/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMCEvent
 * @created     : giovedì feb 13, 2020 12:12:39 CET
 */

#ifndef SLArMCEVENT_HH

#define SLArMCEVENT_HH

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "event/SLArMCPrimaryInfo.hh"
//#include "config/SLArPDSystemConfig.hh"

//#include "event/SLArEventSystemPMT.hh"
//#include "event/SLArEventSystemHodo.hh"

class SLArMCEvent : public TObject
{
  public: 
    SLArMCEvent ();
    ~SLArMCEvent();

    int SetEvNumber    (int nEv);
    int GetEvNumber           ()   {return fEvNumber;}
    //int ConfigPMTSystem (SLArSystemConfigPMT*   pmtSysCfg);
    //int ConfigHodoSystem(SLArSystemConfigHodo* hodoSysCfg);

    //SLArEventSystemPMT*   GetPMTSystem() {return fSystemPMT ;}
    //SLArEventSystemHodo*  GetHodoSystem(){return fSystemHodo;}

    SLArMCPrimaryInfo* GetPrimary() {return fSLArPrimary ;}

    void         Reset         ();

  private:
    int              fEvNumber ;
    SLArMCPrimaryInfo* fSLArPrimary; 
    //SLArEventSystemPMT*     fSystemPMT;
    //SLArEventSystemHodo*    fSystemHodo;

  public:
    ClassDef(SLArMCEvent, 1);
};


#endif /* end of include guard SLArEVENT_HH */

