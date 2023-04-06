/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEventReadoutLinkDef.h
 * @created     : mercoledì ago 10, 2022 15:24:11 CEST
 */

#ifdef __MAKECINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SLArEventGenericHit++; 
#pragma link C++ class SLArEventPhotonHit++; 
#pragma link C++ class SLArEventChargeHit++; 
#pragma link C++ class std::vector<SLArEventChargeHit*>++; 
#pragma link C++ class std::vector<SLArEventPhotonHit*>++;
#pragma link C++ class SLArEventHitsCollection<SLArEventPhotonHit>++;
#pragma link C++ class SLArEventHitsCollection<SLArEventChargeHit>++;
#pragma link C++ class SLArEventChargePixel++; 
#pragma link C++ class std::map<int, SLArEventChargePixel*>++; 
#pragma link C++ class SLArEventTile++;
#pragma link C++ class std::map<int, SLArEventTile*>++;
#pragma link C++ class SLArEventMegatile++;
#pragma link C++ class SLArEventAnode++;
#pragma link C++ class std::map<int, SLArEventAnode*>++;
#pragma link C++ class SLArEventSuperCell++;
#pragma link C++ class std::map<int, SLArEventSuperCell*>++;
#pragma link C++ class SLArEventSuperCellArray++;
#pragma link C++ class std::map<int, SLArEventSuperCellArray*>++;

#endif


