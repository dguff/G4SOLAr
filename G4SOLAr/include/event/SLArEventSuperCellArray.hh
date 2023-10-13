/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventSuperCellArray.hh
 * @created     : Thur Oct 20, 2022 16:08:33 CEST
 */

#ifndef SLAREVENTSUPERCELLARRAY_HH

#define SLAREVENTSUPERCELLARRAY_HH

#include "event/SLArEventSuperCell.hh"
#include "config/SLArCfgSuperCellArray.hh"

template<class S>
class SLArEventSuperCellArray : public TNamed {
  public: 
    SLArEventSuperCellArray(); 
    SLArEventSuperCellArray(SLArCfgSuperCellArray* cfg); 
    SLArEventSuperCellArray(const SLArEventSuperCellArray&); 
    ~SLArEventSuperCellArray();

    int ConfigSystem(SLArCfgSuperCellArray* cfg); 
    inline std::map<int, S>& GetSuperCellMap() {return fSuperCellMap;}
    inline const std::map<int, S>& GetConstSuperCellMap() const {return fSuperCellMap;}
    inline int GetNhits() const {return fNhits;}
    inline bool IsActive() const {return fIsActive;}

    inline void SetLightBacktrackerRecordSize(const UShort_t size) {fLightBacktrackerRecordSize = size;}
    inline UShort_t GetLightBacktrackerRecordSize() const {return fLightBacktrackerRecordSize;}
    S& GetOrCreateEventSuperCell(const int scIdx); 
    S& RegisterHit(const SLArEventPhotonHit& hit); 
    int ResetHits(); 
    int SoftResetHits();

    void SetActive(bool is_active); 
    //bool SortHits(); 

  private: 
    int fNhits; 
    bool fIsActive; 
    UShort_t fLightBacktrackerRecordSize;
    std::map<int, S> fSuperCellMap;

  public:
    ClassDef(SLArEventSuperCellArray, 2); 
}; 

typedef SLArEventSuperCellArray<SLArEventSuperCell*> SLArEventSuperCellArrayPtr;
typedef SLArEventSuperCellArray<std::unique_ptr<SLArEventSuperCell>> SLArEventSuperCellArrayUniquePtr;

#endif /* end of include guard SLAREVENTSUPERCELLARRAY */

