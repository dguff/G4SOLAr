/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArEventTrajectory
 * @created     : lunedì ago 31, 2020 17:30:50 CEST
 */

#ifndef SLArEVENTTRAJECTORY_HH

#define SLArEVENTTRAJECTORY_HH

#include <iostream>
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

struct trj_point {
  float fX; 
  float fY; 
  float fZ;
  float fEdep;
  int   fNph; 
  int   fNel; 

  trj_point() : fX(0.), fY(0.), fZ(0.), fEdep(0.), fNph(0), fNel(0) {}
  trj_point(double x, double y, double z, double edep, int n_ph, int n_el) {
    fX = x; 
    fY = y; 
    fZ = z; 
    fEdep = edep;
    fNph = n_ph; 
    fNel = n_el; 
  }
};



class SLArEventTrajectory : public TObject
{
  public:
    SLArEventTrajectory();
    SLArEventTrajectory(SLArEventTrajectory* trj);
    ~SLArEventTrajectory();

    TString GetParticleName()   {return fParticleName   ;}
    TString GetCreatorProcess() {return fCreatorProcess ;}
    int     GetPDGID()          {return fPDGID          ;}
    int     GetTrackID()        {return fTrackID        ;}
    int     GetParentID()       {return fParentID       ;}
    float   GetInitKineticEne() {return fInitKineticEnergy;}
    float   GetTrakLength()     {return fTrackLength    ;}
    TVector3& GetInitMomentum() {return fInitMomentum   ;}
    float   GetTime()           {return fTime           ;}
    float   GetTotalEdep()      {return fTotalEdep      ;} 
    float   GetTotalNph ()      {return fTotalNph       ;} 
    float   GetTotalNel ()      {return fTotalNel       ;} 

    void    SetParticleName(TString name)   {fParticleName = name;}
    void    SetCreatorProcess(TString proc) {fCreatorProcess = proc;}
    void    SetPDGID       (int pdgID)      {fPDGID    = pdgID   ;}
    void    SetTrackID     (int trkID)      {fTrackID  = trkID   ;}
    void    SetParentID    (int prtID)      {fParentID = prtID   ;}
    void    SetInitKineticEne(float k)      {fInitKineticEnergy=k;}
    void    SetTrackLength (float l)        {fTrackLength = l    ;}
    void    SetInitMomentum(TVector3 p)     {fInitMomentum = p   ;}
    void    SetTime(float t)                {fTime = t           ;}

    std::vector<trj_point>& GetPoints()      {return fTrjPoints   ;}
    void    RegisterPoint(double x, double y, double z, double edep, int n_ph, int n_el);


  private:
    TString                fParticleName     ;
    TString                fCreatorProcess   ; 
    int                    fPDGID            ; 
    int                    fTrackID          ; 
    int                    fParentID         ; 
    float                  fInitKineticEnergy;
    float                  fTrackLength      ; 
    float                  fTime             ; 
    TVector3               fInitMomentum     ;
    std::vector<trj_point> fTrjPoints        ;
    float                  fTotalEdep        ; 
    float                  fTotalNph         ; 
    float                  fTotalNel         ; 

  public:
    ClassDef(SLArEventTrajectory, 2);
};


#endif /* end of include guard SLArEVENTTRAJECTORY_HH */

