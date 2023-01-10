/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArCfgSystemPix
 * @created     Thursday Nov 10, 2022 16:24:26 CET
 */

#include "config/SLArCfgSystemPix.hh"
#include "TList.h"


ClassImp(SLArCfgSystemPix)

SLArCfgSystemPix::SLArCfgSystemPix() : SLArCfgBaseSystem<SLArCfgMegaTile>() {} 

SLArCfgSystemPix::SLArCfgSystemPix(const SLArCfgBaseSystem<SLArCfgMegaTile>& cfg) 
 : SLArCfgBaseSystem<SLArCfgMegaTile>(cfg) 
{}

SLArCfgSystemPix::SLArCfgSystemPix(TString name) 
  : SLArCfgBaseSystem<SLArCfgMegaTile>(name) {}

SLArCfgSystemPix::SLArPixIdxCoord SLArCfgSystemPix::FindPixel(double x, double y) {
  SLArCfgSystemPix::SLArPixIdxCoord pidx = {-9}; 

  int ibin = fAnodeLevelsMap[0]->FindBin(x, y); 

  SLArCfgMegaTile* megatile = FindBaseElementInMap(ibin);

  if (megatile) {
    pidx[0] = megatile->GetBinIdx();  
    Double_t mt_x = megatile->GetZ(); 
    Double_t mt_y = megatile->GetY(); 
    //printf("correct for MT %s coordinates: %g, %g mm -> %g, %g \n", 
        //megatile->GetName(), mt_x, mt_y, x-mt_x, y-mt_y);
    //getchar(); 
    ibin = fAnodeLevelsMap[1]->FindBin(x-mt_x, y-mt_y); 
    //printf("Tile map bin: %i\n", ibin);
    SLArCfgReadoutTile* tile = megatile->FindBaseElementInMap(ibin); 
    if (tile) {
      pidx[1] = tile->GetBinIdx(); 
      Double_t t_x = tile->GetPhysZ(); 
      Double_t t_y = tile->GetPhysY(); 
      pidx[2] = fAnodeLevelsMap[2]->FindBin(x-t_x, y-t_y); 
    } 
//#ifdef SLAR_DEBUG
    //else {
      //printf("Cannot find tile with bin index %i\n", ibin);
    //}
//#endif

  } 
#ifdef SLAR_DEBUG
  else {
    printf("SLArCfgSystemPix::FindPixel(%g, %g)\n", x, y);
    printf("Coordinates outside of anode scope\n"); 
  }
#endif

  return pidx; 
}

void SLArCfgSystemPix::RegisterMap(size_t ilevel, TH2Poly* hmap) {
  if (fAnodeLevelsMap.count(ilevel)) {
    printf("SLArCfgSystemPix::RegisterMap(%lu) ERROR\n", ilevel); 
    printf("A map is already registered for level %lu. Quit.\n", ilevel);
    exit(1);
  }

  fAnodeLevelsMap.insert(std::make_pair(ilevel, hmap)); 
  return;
}

TH2Poly* SLArCfgSystemPix::ConstructPixHistMap(const int idxMT, const int idxT)
{
  SLArCfgMegaTile* cfgMegaTile  = GetBaseElement(idxMT); 
  if (!cfgMegaTile) return nullptr; 
  SLArCfgReadoutTile* cfgTile = cfgMegaTile->GetBaseElement(idxT);
  double tile_xpos = cfgTile->GetPhysZ(); 
  double tile_ypos = cfgTile->GetPhysY(); 

  TString name_ = Form("hqpix_mt%i_t%i", idxMT, idxT); 
  TH2Poly* h2 = new TH2Poly(name_.Data(),
      Form("%s;#it{z} [mm];#it{y} [mm]", name_.Data()),
      tile_xpos-0.5*cfgTile->Get2DSize_X(), 
      tile_xpos+0.5*cfgTile->Get2DSize_X(),
      tile_ypos-0.5*cfgTile->Get2DSize_Y(), 
      tile_ypos+0.5*cfgTile->Get2DSize_Y()
      ); 
  h2->SetFloat(); 

  TH2Poly* h2_template = fAnodeLevelsMap.find(2)->second;
  for (const auto& bbin : *(h2_template->GetBins())) {
    TH2PolyBin* bin = (TH2PolyBin*)bbin;
    TGraph* g_tpl = (TGraph*)bin->GetPolygon();
    TGraph* g = (TGraph*)g_tpl->Clone(); 
    for (int i=0; i<g->GetN(); i++) {
      g->GetX()[i] += tile_xpos; 
      g->GetY()[i] += tile_ypos; 
    }
    h2->AddBin(g); 
  }


  return h2;
}
