/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMCPrimaryInfo
 * @created     : venerdì feb 14, 2020 16:43:28 CET
 */

#ifndef SLArMCPRIMARYINFO_HH

#define SLArMCPRIMARYINFO_HH

#include <iostream>
#include "TNamed.h"
#include "TH3F.h"
#include "event/SLArEventTrajectory.hh"

class SLArMCPrimaryInfo : public TNamed 
{
  public:
    SLArMCPrimaryInfo();
    ~SLArMCPrimaryInfo();

    void SetPosition(double  x, double  y, double  z, double t = 0);
    void SetMomentum(double px, double py, double pz, double   ene);
    void SetID      (int           id) {fID    =   id;}
    void SetTrackID (int           id) {fTrkID =   id;}
    void SetName    (const char* name) {fName = name;}
    void SetTotalEdep   (float edep)   {fTotalEdep    = edep;}

    TString   GetParticleName() {return fName     ;}
    std::vector<double>   GetMomentum    () {return fMomentum ;}
    std::vector<double>   GetVertex      () {return fVertex   ;}
    double    GetEnergy      () {return fEnergy   ;}
    int       GetCode        () {return fID       ;}
    double    GetTime        () {return fTime     ;}
    double    GetTotalEdep   () {return fTotalEdep;}
    int       GetID          () {return fID       ;}
    int       GetTrackID     () {return fTrkID    ;}
    std::vector<SLArEventTrajectory*>&
              GetTrajectories() {return fTrajectories;}
    

    void PrintParticle();

    void ResetParticle();
    
    int RegisterTrajectory(SLArEventTrajectory* trj);

  private:
    int      fID      ; 
    int      fTrkID   ;
    TString  fName    ; 
    double   fEnergy  ;
    double   fTime    ;
    double   fTotalEdep;
    std::vector<double>   fVertex  ;
    std::vector<double>   fMomentum;
    std::vector<SLArEventTrajectory*> fTrajectories;
  
  public:
    ClassDef(SLArMCPrimaryInfo, 1);
};



#endif /* end of include guard SLArMCTRACKINFO_HH */

