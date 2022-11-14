/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventPhotonHit.hh
 * @created     : mercoledì ago 10, 2022 12:08:32 CEST
 */

#ifndef SLAREVENTPHOTONHIT_HH

#define SLAREVENTPHOTONHIT_HH

#include "event/SLArEventGenericHit.hh"

enum  EPhProcess {kAll = 0, kCher = 1, kScnt = 2, kWLS = 3};
extern TString EPhProcName[4];
extern TString EPhProcTitle[4];

class SLArEventPhotonHit : public SLArEventGenericHit  
{
  public:
    SLArEventPhotonHit();
    SLArEventPhotonHit(float time, EPhProcess proc, float wvl = 0);
    SLArEventPhotonHit(float time, int proc, float wvl = 0);
    SLArEventPhotonHit(const SLArEventPhotonHit &pmtHit);
    ~SLArEventPhotonHit() {}

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
    float GetWavelength() {return fWavelength;}
    int   GetProcess() {return fProcess;}
    float* GetLocalPos() {return fLocPos ;}

    void  DumpInfo  ();

  private:
    int          fMegaTileIdx; 
    int          fRowTileNr; 
    int          fTileNr; 
    float        fWavelength;
    float        fLocPos[3];
    EPhProcess   fProcess;

    ClassDef(SLArEventPhotonHit, 2);
};

#endif /* end of include guard SLAREVENTPHOTONHIT_HH */

