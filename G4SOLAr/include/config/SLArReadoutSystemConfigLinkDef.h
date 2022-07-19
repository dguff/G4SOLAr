#ifdef __MAKECINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SLArCfgBaseModule++;
#pragma link C++ class SLArCfgSuperCell++;
#pragma link C++ class SLArCfgReadoutTile++;
#pragma link C++ class std::map<int, SLArCfgSuperCell*>++;
#pragma link C++ class std::map<int, SLArCfgReadoutTile*>++;
#pragma link C++ class SLArCfgAssembly<SLArCfgSuperCell>++; 
#pragma link C++ class SLArCfgAssembly<SLArCfgReadoutTile>++; 
#pragma link C++ class SLArCfgSuperCellArray++; 
#pragma link C++ class SLArCfgMegaTile++; 
#pragma link C++ class std::map<TString, SLArCfgSuperCellArray*>++;
#pragma link C++ class std::map<TString, SLArCfgMegaTile*>++;
#pragma link C++ class SLArCfgBaseSystem<SLArCfgSuperCellArray>++;
#pragma link C++ class SLArCfgBaseSystem<SLArCfgMegaTile>++;
#endif

