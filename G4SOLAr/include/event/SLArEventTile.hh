/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArEventTile.hh
 * @created     Wed Aug 10, 2022 12:13:26 CEST
 */

#ifndef SLAREVENTTILE_HH

#define SLAREVENTTILE_HH

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include "event/SLArEventHitsCollection.hh"
#include "event/SLArEventChargePixel.hh"
#include "event/SLArEventPhotonHit.hh"

template<class P>
class SLArEventTile :  public SLArEventHitsCollection<SLArEventPhotonHit> 
{
  public: 
    SLArEventTile(); 
    SLArEventTile(const int idx); 
    SLArEventTile(const SLArEventTile& ev); 
    ~SLArEventTile(); 
    template<class T>
    void SoftCopy(SLArEventTile<T>& record) const; 

    double GetTime() const;
    double GetTime(EPhProcess proc) const;
    inline std::map<int, P>& GetPixelEvents() {return fPixelHits;}
    inline const std::map<int, P>& GetConstPixelEvents() const {return fPixelHits;}
    inline double GetNPixelHits() const {return fPixelHits.size();}
    double GetPixelHits() const; 
    inline void SetChargeBacktrackerRecordSize(const UShort_t size) {fChargeBacktrackerRecordSize = size;}
    inline UShort_t GetChargeBacktrackerRecordSize() const {return fChargeBacktrackerRecordSize;}
    void PrintHits() const; 
    P& RegisterChargeHit(const int&, const SLArEventChargeHit& ); 
    int ResetHits(); 
    int SoftResetHits();

    //bool SortHits(); 
    //bool SortPixelHits();

  protected:
    UShort_t fChargeBacktrackerRecordSize;
    std::map<int, P> fPixelHits; 

  public:
     ClassDef(SLArEventTile, 2)
};

typedef SLArEventTile<SLArEventChargePixel*> SLArEventTilePtr;
typedef SLArEventTile<std::unique_ptr<SLArEventChargePixel>> SLArEventTileUniquePtr;

#endif /* end of include guard SLAREVENTTILE_HH */

