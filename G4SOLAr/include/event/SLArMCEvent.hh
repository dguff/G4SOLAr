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



class SLArMCEvent : public TObject
{
  public: 
    enum EDirectionMode {kFixed = 0, kRandom = 1};

    SLArMCEvent ();
    ~SLArMCEvent();

    int SetEvNumber    (int nEv);
    int GetEvNumber           ()   {return fEvNumber;}

    void SetDirectionMode(EDirectionMode kMode) {fDirectionMode = kMode;}
    void SetDirection(double* dir = nullptr); 
    void SetDirection(double px, double py, double pz); 
    EDirectionMode GetDirectionMode() {return fDirectionMode;}
    std::array<double, 3> GetDirection() {return fDirection;}
    //int ConfigPMTSystem (SLArSystemConfigPMT*   pmtSysCfg);
    //int ConfigHodoSystem(SLArSystemConfigHodo* hodoSysCfg);

    //SLArEventSystemPMT*   GetPMTSystem() {return fSystemPMT ;}
    //SLArEventSystemHodo*  GetHodoSystem(){return fSystemHodo;}

    std::vector<SLArMCPrimaryInfo*>& GetPrimaries() {return fSLArPrimary ;}
    SLArMCPrimaryInfo* GetPrimary(int ip) {return fSLArPrimary.at(ip);}
    bool  CheckIfPrimary(int trkId);

    inline size_t RegisterPrimary(SLArMCPrimaryInfo* p) 
      {fSLArPrimary.push_back(p); return fSLArPrimary.size();}

    void  Reset();

  private:
    int fEvNumber;
    std::array<double, 3>  fDirection; 
    std::vector<SLArMCPrimaryInfo*> fSLArPrimary; 
    EDirectionMode fDirectionMode;
    //SLArEventSystemPMT*     fSystemPMT;
    //SLArEventSystemHodo*    fSystemHodo;

  public:
    ClassDef(SLArMCEvent, 3);
};


#endif /* end of include guard SLArEVENT_HH */

