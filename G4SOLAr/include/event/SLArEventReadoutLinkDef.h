/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventReadoutLinkDef.h
 * @created     : Wed Aug 10, 2022 15:24:11 CEST
 */

#ifdef __MAKECINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SLArEventGenericHit++; 
#pragma link C++ class SLArEventPhotonHit++; 
#pragma link C++ class SLArEventChargeHit++; 
#pragma link C++ class std::vector<SLArEventChargeHit>++; 
#pragma link C++ class std::vector<SLArEventPhotonHit>++;
#pragma link C++ class SLArEventBacktrackerRecord++;
#pragma link C++ class std::vector<SLArEventBacktrackerRecord>++;
#pragma link C++ class std::map<Int_t, UShort_t>++;
#pragma link C++ typedef BacktrackerCounter_t++;
#pragma link C++ class SLArEventBacktrackerVector++;
#pragma link C++ class std::map<UShort_t, SLArEventBacktrackerVector>++;
#pragma link C++ typedef BacktrackerVectorCollection_t++;
#pragma link C++ class std::map<UShort_t, UShort_t>++;
#pragma link C++ typedef HitsCollection_t++;
#pragma link C++ class SLArEventHitsCollection<SLArEventPhotonHit>++;
#pragma link C++ class SLArEventHitsCollection<SLArEventChargeHit>++;
#pragma link C++ class SLArEventChargePixel++; 
#pragma link C++ class std::map<int, std::unique_ptr<SLArEventChargePixel>>++; 
#pragma link C++ class std::map<int, SLArEventChargePixel*>++; 
#pragma link C++ class SLArEventTile<std::unique_ptr<SLArEventChargePixel>>++;
#pragma link C++ class SLArEventTile<SLArEventChargePixel*>++;
#pragma link C++ typedef SLArEventTilePtr++;
#pragma link C++ typedef SLArEventTileUniquePtr++;
#pragma link C++ class std::map<int, std::unique_ptr<SLArEventTileUniquePtr>>++;
#pragma link C++ class std::map<int, SLArEventTilePtr*>++;
#pragma link C++ class SLArEventMegatile<std::unique_ptr<SLArEventTileUniquePtr>>++;
#pragma link C++ class SLArEventMegatile<SLArEventTilePtr*>++;
#pragma link C++ typedef SLArEventMegatilePtr++;
#pragma link C++ typedef SLArEventMegatileUniquePtr++;
#pragma link C++ class std::map<int, std::unique_ptr<SLArEventMegatileUniquePtr>>++;
#pragma link C++ class std::map<int, SLArEventMegatilePtr*>++;
#pragma link C++ class SLArEventAnode<std::unique_ptr<SLArEventMegatileUniquePtr>, std::unique_ptr<SLArEventTileUniquePtr>, std::unique_ptr<SLArEventChargePixel>>++;
#pragma link C++ class SLArEventAnode<SLArEventMegatilePtr*, SLArEventTilePtr*, SLArEventChargePixel*>++;
#pragma link C++ typedef SLArEventAnodePtr++;
#pragma link C++ typedef SLArEventAnodeUniquePtr++;
#pragma link C++ class SLArEventSuperCell++;
#pragma link C++ class std::map<int, std::unique_ptr<SLArEventSuperCell>>++;
#pragma link C++ class std::map<int, SLArEventSuperCell*>++;
#pragma link C++ class SLArEventSuperCellArray<std::unique_ptr<SLArEventSuperCell>>++;
#pragma link C++ class SLArEventSuperCellArray<SLArEventSuperCell*>++;
#pragma link C++ typedef SLArEventSuperCellArrayPtr++;
#pragma link C++ typedef SLArEventSuperCellArrayUniquePtr++;

#endif


