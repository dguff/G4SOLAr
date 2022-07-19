/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgMegaTile.hh
 * @created     : martedì lug 19, 2022 10:15:59 CEST
 */

#include "config/SLArCfgMegaTile.hh"

ClassImp(SLArCfgMegaTile)

SLArCfgMegaTile::SLArCfgMegaTile()
  : SLArCfgAssembly<SLArCfgReadoutTile>()
{
  SetName("aMegaTileHasNoName");    
}

SLArCfgMegaTile::SLArCfgMegaTile(TString name, int serie) 
  : SLArCfgAssembly<SLArCfgReadoutTile>(name, serie) 
{
  SetName(name);
  fSerie = serie;
  printf("SLArCfgMegaTile created with name %s\n", fName.Data());
}

SLArCfgMegaTile::SLArCfgMegaTile(const SLArCfgMegaTile &cfg)
  : SLArCfgAssembly<SLArCfgReadoutTile>(cfg)
{}

SLArCfgMegaTile::~SLArCfgMegaTile()
{
  if (fH2Bins) {delete fH2Bins; fH2Bins = nullptr;}
  for (auto &sc : fElementsMap)
    if (sc.second) {delete sc.second; sc.second = 0;}
  fElementsMap.clear();
  fNElements = 0;
}

void SLArCfgMegaTile::DumpMap() 
{
  std::printf("SLArCfgMegaTile %s has %i entries\n", 
      fName.Data(), (int)fElementsMap.size());
  for (auto &itr : fElementsMap)
    itr.second->DumpInfo();
}

