/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventTile.hh
 * @created     : mercoledì ago 10, 2022 12:13:26 CEST
 */

#ifndef SLAREVENTTILE_HH

#define SLAREVENTTILE_HH

#include <iostream>
#include <vector>
#include <map>
#include "event/SLArEventHitsCollection.hh"
#include "event/SLArEventChargePixel.hh"
#include "event/SLArEventPhotonHit.hh"

class SLArEventTile :  public SLArEventHitsCollection<SLArEventPhotonHit> 
{
  public: 
    SLArEventTile(); 
    SLArEventTile(int idx); 
    SLArEventTile(const SLArEventTile& ev); 
    ~SLArEventTile(); 

    double GetTime();
    double GetTime(EPhProcess proc);
    std::map<int, SLArEventChargePixel*>& GetPixelEvents() {return fPixelHits;}
    double GetPixelHits() {return fPixelHits.size();}

    void PrintHits(); 
    int RegisterChargeHit(int, SLArEventChargeHit* ); 
    int ResetHits(); 

    bool SortHits(); 
    bool SortPixelHits();

  protected:
    std::map<int, SLArEventChargePixel*> fPixelHits; 

  public:
     ClassDef(SLArEventTile, 2)
};

#endif /* end of include guard SLAREVENTTILE_HH */

