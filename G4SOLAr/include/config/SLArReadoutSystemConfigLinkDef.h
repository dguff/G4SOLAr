#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SLArCfgBaseModule+;
#pragma link C++ class SLArCfgSuperCell+;
#pragma link C++ class SLArCfgReadoutTile+;
#pragma link C++ class std::vector<SLArCfgSuperCell>+;
#pragma link C++ class std::vector<SLArCfgReadoutTile>+;
#pragma link C++ class SLArCfgAssembly<SLArCfgSuperCell>+; 
#pragma link C++ class SLArCfgAssembly<SLArCfgReadoutTile>+; 
#pragma link C++ class SLArCfgSuperCellArray+; 
#pragma link C++ class SLArCfgMegaTile+; 
#pragma link C++ class std::vector<SLArCfgMegaTile>+;
#pragma link C++ class SLArCfgAssembly<SLArCfgMegaTile>+;
#pragma link C++ class std::map<size_t, TH2Poly*>+;
#pragma link C++ class SLArCfgAnode+;
#pragma link C++ class std::vector<SLArCfgSuperCellArray>+;
#pragma link C++ class SLArCfgBaseSystem<SLArCfgSuperCellArray>+;
#endif

