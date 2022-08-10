/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventPhotonHit.hh
 * @created     : mercoledì ago 10, 2022 12:08:32 CEST
 */

#ifndef SLAREVENTPHOTONHIT_HH

#define SLAREVENTPHOTONHIT_HH

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <algorithm>

enum  EPhProcess {kAll = 0, kCher = 1, kScnt = 2, kWLS = 3};
extern TString EPhProcName[4];
extern TString EPhProcTitle[4];

class SLArEventPhotonHit : public TObject 
{
  public:
    SLArEventPhotonHit();
    SLArEventPhotonHit(float time, EPhProcess proc, float wvl = 0);
    SLArEventPhotonHit(float time, int proc, float wvl = 0);
    SLArEventPhotonHit(const SLArEventPhotonHit &pmtHit);
    ~SLArEventPhotonHit();

    void  SetTime    (float      t) {fTime    = t;}
    void  SetProcess (EPhProcess p) {fProcess = p;}
    void  SetLocalPos(float x, float y, float z);
    void  SetWavelength(float w) {fWavelength = w;}
    void  SetMegaTileIdx(int id) {fMegaTileIdx = id;}
    void  SetRowTileNr(int id) {fRowTileNr = id;}
    void  SetTileNr(int id) {fTileNr = id;}
    void  SetTileInfo(int mg, int row, int tile); 

    int   GetMegaTileIdx() {return fMegaTileIdx;}
    int   GetRowTileNr() {return fRowTileNr;}
    int   GetTileNr() {return fTileNr;}
    int   GetTileIdx() {return (fRowTileNr+1)*100 + fTileNr;}
    float GetTime() {return fTime;}
    float GetWavelength() {return fWavelength;}
    int   GetProcess() {return fProcess;}
    float* GetLocalPos() {return fLocPos ;}

    void  DumpInfo  ();

    bool  operator< (const SLArEventPhotonHit &other) const 
                    {return fTime < other.fTime;}

    static bool  CompareHitPtrs (const SLArEventPhotonHit* left, 
                          const SLArEventPhotonHit* right) 
                    {return left->fTime < right->fTime;}
  private:
    int          fMegaTileIdx; 
    int          fRowTileNr; 
    int          fTileNr; 
    float        fTime;
    float        fWavelength;
    float        fLocPos[3];
    EPhProcess   fProcess;


    ClassDef(SLArEventPhotonHit, 1);
};

#endif /* end of include guard SLAREVENTPHOTONHIT_HH */

