/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArMCPrimaryInfo.hh
 * @created     Fri Feb 14, 2020 16:43:28 CET
 */

#ifndef SLArMCPRIMARYINFO_HH

#define SLArMCPRIMARYINFO_HH

#include <iostream>
#include <vector>
#include <memory>
#include "TNamed.h"
#include "event/SLArEventTrajectory.hh"

class SLArMCPrimaryInfo : public TNamed 
{
  public:
    SLArMCPrimaryInfo();
    SLArMCPrimaryInfo(const SLArMCPrimaryInfo& p);
    ~SLArMCPrimaryInfo();

    void SetPosition(const double&  x, const double&  y, const double&  z, const double& t = 0);
    void SetMomentum(const double& px, const double& py, const double& pz, const double&   ene);
    inline void SetID(const int& id) {fID = id;}
    inline void SetTrackID(const int& id) {fTrkID = id;}
    inline void SetTime(const double& time) {fTime = time;}
    inline void SetTotalEdep(const float& edep) {fTotalEdep = edep;}
    inline void SetTotalLArEdep(const float& edep) {fTotalLArEdep = edep;}
    inline void SetTotalScintPhotons(const int& nph) {fTotalScintPhotons = nph;}
    inline void SetTotalCerenkovPhotons(const int& nph) {fTotalCerenkovPhotons = nph;}

    inline TString GetParticleName() const {return fName;}
    inline std::vector<double> GetMomentum() const {return fMomentum;}
    inline std::vector<double> GetVertex() const {return fVertex;}
    inline double GetEnergy() const {return fEnergy;}
    inline int GetCode() const {return fID;}
    inline double GetTime() const {return fTime;}
    inline double GetTotalEdep() const {return fTotalEdep;}
    inline double GetTotalLArEdep() const {return fTotalLArEdep;}
    inline int GetID() const {return fID;}
    inline int GetTrackID() const {return fTrkID;}
    inline std::vector<std::unique_ptr<SLArEventTrajectory>>& GetTrajectories() {return fTrajectories;}
    inline const std::vector<std::unique_ptr<SLArEventTrajectory>>& GetConstTrajectories() const {return fTrajectories;}
    inline int GetTotalScintPhotons() const {return fTotalScintPhotons;}
    inline int GetTotalCerenkovPhotons() const {return fTotalCerenkovPhotons;}

    inline void IncrementLArEdep(const double edep) {fTotalLArEdep += edep;}
    inline void IncrementScintPhotons() {fTotalScintPhotons++;}
    inline void IncrementScintPhotons(const size_t n) {fTotalScintPhotons += n;}
    inline void IncrementCherPhotons() {fTotalCerenkovPhotons++;}
    inline void IncrementCherPhotons(const size_t n) {fTotalCerenkovPhotons += n;}

    void PrintParticle() const;

    void ResetParticle();
    void SoftResetParticle();
    
    int RegisterTrajectory(std::unique_ptr<SLArEventTrajectory> trj);

  private:
    Int_t fID; 
    Int_t fTrkID;
    Double_t fEnergy;
    Double_t fTime;
    Double_t fTotalEdep;
    Int_t fTotalScintPhotons;
    Int_t fTotalCerenkovPhotons;
    Double_t fTotalLArEdep; 
    std::vector<Double_t> fVertex;
    std::vector<Double_t> fMomentum;
    std::vector<std::unique_ptr<SLArEventTrajectory>> fTrajectories;
  
  public:
    ClassDef(SLArMCPrimaryInfo, 3);
};

#endif /* end of include guard SLArMCTRACKINFO_HH */

