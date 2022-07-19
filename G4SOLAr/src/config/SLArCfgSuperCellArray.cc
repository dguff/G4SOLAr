/**
 * @author      : guff (guff@guff-gssi)
 * @file        : BCCfgArrayPMT
 * @created     : giovedì giu 04, 2020 11:47:56 CEST
 */

#include "config/SLArCfgSuperCellArray.hh"

ClassImp(SLArCfgSuperCellArray)

SLArCfgSuperCellArray::SLArCfgSuperCellArray() 
  : SLArCfgAssembly<SLArCfgSuperCell>()
{
  SetName("aPMTArrayHasNoName");
}

SLArCfgSuperCellArray::SLArCfgSuperCellArray(TString name, int serie) 
  : SLArCfgAssembly<SLArCfgSuperCell>(name, serie) 
{
  SetName(name);
  fSerie = serie;
  printf("SLArCfgSuperCellArray created with name %s\n", fName.Data());
}


SLArCfgSuperCellArray::SLArCfgSuperCellArray(const SLArCfgSuperCellArray &cfg)
  : SLArCfgAssembly<SLArCfgSuperCell>(cfg)
{}

SLArCfgSuperCellArray::~SLArCfgSuperCellArray()
{
  if (fH2Bins) {delete fH2Bins; fH2Bins = nullptr;}
  for (auto &sc : fElementsMap)
    if (sc.second) {delete sc.second; sc.second = 0;}
  fElementsMap.clear();
  fNElements = 0;
}

void SLArCfgSuperCellArray::DumpMap() 
{
  std::printf("SLArCfgSuperCellArray %s has %i entries\n", 
      fName.Data(), (int)fElementsMap.size());
  for (auto &itr : fElementsMap)
    itr.second->DumpInfo();
}

