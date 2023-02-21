/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArEventHitsCollection.cc
 * @created     Fri Nov 11, 2022 14:21:29 CET
 */

#ifndef SLAREVENTHITSCOLLECTION_HH

#define SLAREVENTHITSCOLLECTION_HH

#include <iostream>
#include <vector>

#include "TNamed.h"
#include "event/SLArEventGenericHit.hh"

template<class T>
class SLArEventHitsCollection : public TNamed {
  public: 
    SLArEventHitsCollection(); 
    SLArEventHitsCollection(int); 
    SLArEventHitsCollection(const SLArEventHitsCollection&); 
    virtual ~SLArEventHitsCollection(); 

    int GetIdx() {return fIdx;}
    int GetNhits() {return fNhits;}
    virtual double GetTime() {return -1.;} 
    std::vector<T*>& GetHits() {return fHits;}

    bool IsActive() {return fIsActive;}

    virtual void PrintHits(); 

    virtual int RegisterHit(T* hit); 
    virtual int ResetHits(); 

    virtual bool SortHits(); 
    void SetActive(bool is_active) {fIsActive = is_active;}
    void SetIdx(int idx) {fIdx = idx;}


  protected:
    int fIdx; 
    bool fIsActive; 
    int fNhits; 
    std::vector<T*> fHits; 

  public: 
    ClassDefOverride(SLArEventHitsCollection, 1);
}; 



#endif /* end of include guard SLAREVENTHITSCOLLECTION_HH */

