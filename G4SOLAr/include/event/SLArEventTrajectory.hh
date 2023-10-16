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
  float fEdep;
  int   fNph; 
  int   fNel; 
  int   fCopy; 
  bool  fLAr;

  trj_point() : fX(0.), fY(0.), fZ(0.), fKEnergy(0.), fEdep(0.), fNph(0), fNel(0), fCopy(0), fLAr(false) {}

  trj_point(double x, double y, double z, double energy, double edep, int n_ph = 0, int n_el = 0, int copy = 0, bool in_lar = false) {
    fX = x; 
    fY = y; 
    fZ = z; 
    fKEnergy = energy;
    fEdep = edep; 
    fNph = n_ph; 
    fNel = n_el; 
    fCopy = copy;
    fLAr = in_lar;
  }

};



class SLArEventTrajectory : public TObject
{
  public:
    SLArEventTrajectory();
    SLArEventTrajectory(const SLArEventTrajectory& trj);
    ~SLArEventTrajectory();

    TString GetParticleName() const {return fParticleName;}
    TString GetCreatorProcess() const {return fCreatorProcess ;}
    TString GetEndProcess() const {return fEndProcess ;}
    int     GetPDGID() const {return fPDGID ;}
    int     GetTrackID() const {return fTrackID;}
    int     GetParentID() const {return fParentID;}
    float   GetInitKineticEne() const {return fInitKineticEnergy;}
    float   GetTrakLength() const {return fTrackLength;}
    const TVector3& GetInitMomentum() const {return fInitMomentum;}
    float GetTime()      const {return fTime;}
    float GetWeight() const {return fWeight;}
    float GetTotalEdep() const {return fTotalEdep;} 
    float GetTotalNph () const {return fTotalNph;} 
    float GetTotalNel () const {return fTotalNel;} 
    Bool_t DoStoreTrajectoryPts() const {return fStoreTrajectoryPts;}

    inline void SetStoreTrajectoryPts(const bool store_pts) {fStoreTrajectoryPts = store_pts;}
    inline void SetParticleName(const TString& name) {fParticleName = name;}
    inline void SetCreatorProcess(const TString& proc) {fCreatorProcess = proc;}
    inline void SetEndProcess(const TString& proc) {fEndProcess = proc;}
    inline void SetPDGID(const int& pdgID) {fPDGID = pdgID;}
    inline void SetTrackID(const int& trkID) {fTrackID = trkID;}
    inline void SetParentID(const int& prtID) {fParentID = prtID;}
    inline void SetInitKineticEne(const float& k) {fInitKineticEnergy=k;}
    inline void SetTrackLength(const float& l) {fTrackLength = l;}
    inline void SetInitMomentum(const TVector3& p) {fInitMomentum = p;}
    inline void SetTime(const float& t) {fTime = t;}
    inline void SetWeight(const float& w) {fWeight = w;}
    inline void IncrementEdep(const double& edep) {fTotalEdep += edep;}
    inline void IncrementNion(const int& nion) {fTotalNel += nion;}
    inline void IncrementNph(const int& nph) {fTotalNph += nph;}

    std::vector<trj_point>& GetPoints()      {return fTrjPoints;}
    const std::vector<trj_point>& GetConstPoints()  const {return fTrjPoints;}
    void    RegisterPoint(double x, double y, double z, double ene, double edep, int n_ph, int n_el, int copy);
    void    RegisterPoint(const trj_point& point); 


  private:
    Bool_t                 fStoreTrajectoryPts;
    TString                fParticleName     ; 
    TString                fCreatorProcess   ; 
    TString                fEndProcess       ;
    int                    fPDGID            ; 
    int                    fTrackID          ; 
    int                    fParentID         ; 
    float                  fInitKineticEnergy;
    float                  fTrackLength      ; 
    float                  fTime             ; 
    float                  fWeight           ;
    TVector3               fInitMomentum     ;
    std::vector<trj_point> fTrjPoints        ;
    float                  fTotalEdep        ; 
    float                  fTotalNph         ; 
    float                  fTotalNel         ; 

  public:
    ClassDef(SLArEventTrajectory, 4);
};


#endif /* end of include guard SLArEVENTTRAJECTORY_HH */

