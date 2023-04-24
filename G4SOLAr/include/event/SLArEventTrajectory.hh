/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArEventTrajectory.hh
 * @created     Mon Aug 31, 2020 17:30:50 CEST
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
  float fKEnergy; 
  int   fNph; 
  int   fNel; 
  int   fCopy; 

  trj_point() : fX(0.), fY(0.), fZ(0.), fKEnergy(0.), fNph(0), fNel(0), fCopy(0) {}
  trj_point(double x, double y, double z, double energy, int n_ph = 0, int n_el = 0, int copy = 0) {
    fX = x; 
    fY = y; 
    fZ = z; 
    fKEnergy = energy;
    fNph = n_ph; 
    fNel = n_el; 
    fCopy = copy;
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
    void    IncrementEdep(double edep)      {fTotalEdep += edep  ;}
    void    IncrementNion(int nion)         {fTotalNel += nion   ;}
    void    IncrementNph(int nph)           {fTotalNph += nph    ;}

    std::vector<trj_point>& GetPoints()      {return fTrjPoints   ;}
    void    RegisterPoint(double x, double y, double z, double ene, double edep, int n_ph, int n_el, int copy);
    void    RegisterPoint(const trj_point& point); 


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

