/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgSystemPix
 * @created     : giovedì nov 10, 2022 16:24:26 CET
 */

#include "config/SLArCfgSystemPix.hh"


ClassImp(SLArCfgSystemPix)

SLArCfgSystemPix::SLArCfgSystemPix() : SLArCfgBaseSystem<SLArCfgMegaTile>() {} 

SLArCfgSystemPix::SLArCfgSystemPix(const SLArCfgBaseSystem<SLArCfgMegaTile>& cfg) 
 : SLArCfgBaseSystem<SLArCfgMegaTile>(cfg) 
{}

SLArCfgSystemPix::SLArCfgSystemPix(TString name) 
  : SLArCfgBaseSystem<SLArCfgMegaTile>(name) {}

SLArCfgSystemPix::SLArPixIdxCoord SLArCfgSystemPix::FindPixel(double x, double y) {
  SLArCfgSystemPix::SLArPixIdxCoord pidx = {-9}; 
  if (!fH2Bins) BuildPolyBinHist(); 

  int ibin = fH2Bins->Fill(x, y); 

  SLArCfgMegaTile* megatile = FindBaseElementInMap(ibin);

  if (megatile) {
    pidx[0] = megatile->GetBinIdx();  
    SLArCfgReadoutTile* tile = megatile->FindPixel(x, y); 
    if (tile) {
      pidx[1] = tile->GetBinIdx(); 
      pidx[2] = tile->FindPixel(x, y); 
    }
  }

  return pidx; 
}
