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
    void SetID      (int           id) {fID   =   id;}
    void SetName    (const char* name) {fName = name;}
    void SetNCherPhotons(int n_ph)     {fNCherPhotons = n_ph;}
    void SetNScntPhotons(int n_ph)     {fNScntPhotons = n_ph;}
    void SetNWLSPhotons (int n_ph)     {fNWLSPhotons  = n_ph;}
    void SetTotalEdep   (float edep)   {fTotalEdep    = edep;}

    TString   GetParticleName() {return fName    ;}
    double*   GetMomentum    () {return fMomentum;}
    double*   GetVertex      () {return fVertex  ;}
    double    GetEnergy      () {return fEnergy  ;}
    int       GetCode        () {return fID      ;}
    double    GetTime        () {return fTime    ;}
    double    GetNCherPhotons() {return fNCherPhotons;}
    double    GetNScntPhotons() {return fNScntPhotons;}
    double    GetNWLSPhotons () {return fNWLSPhotons ;}
    double    GetTotalEdep   () {return fTotalEdep   ;}
    std::vector<SLArEventTrajectory*>&
              GetTrajectories() {return fTrajectories;}
    TH3F*     GetEdep3Hist   () {return fEdep3Hist   ;}
    void      AddEdep        (float x, float y, float z, float edep);
    

    void PrintParticle();

    void ResetParticle();
    
    int RegisterTrajectory(SLArEventTrajectory* trj);

  private:
    int      fID      ; 
    TString  fName    ; 
    double   fEnergy  ;
    double   fTime    ;
    double   fTotalEdep;
    int      fNCherPhotons; 
    int      fNScntPhotons;
    int      fNWLSPhotons ; 
    double   fVertex  [3];
    double   fMomentum[3];
    std::vector<SLArEventTrajectory*> fTrajectories;
    TH3F*    fEdep3Hist;

  
  public:
    ClassDef(SLArMCPrimaryInfo, 2);
};



#endif /* end of include guard SLArMCTRACKINFO_HH */

