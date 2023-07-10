/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArCfgAnode.cc
 * @created     Thursday Nov 10, 2022 16:24:26 CET
 */

#include "config/SLArCfgAnode.hh"
#include "TList.h"


ClassImp(SLArCfgAnode)

SLArCfgAnode::SLArCfgAnode() 
  : SLArCfgAssembly<SLArCfgMegaTile>(), 
  fTPCID(0)
{} 

SLArCfgAnode::SLArCfgAnode(const SLArCfgAssembly<SLArCfgMegaTile>& cfg) 
 : SLArCfgAssembly<SLArCfgMegaTile>(cfg), fTPCID(0)
{}

SLArCfgAnode::SLArCfgAnode(TString name) 
  : SLArCfgAssembly<SLArCfgMegaTile>(name), fTPCID(0)
{}

SLArCfgAnode::~SLArCfgAnode() {
  for (auto &hmap : fAnodeLevelsMap) {delete hmap.second; hmap.second = 0;}
  fAnodeLevelsMap.clear(); 
}

SLArCfgAnode::SLArPixIdxCoord SLArCfgAnode::FindPixel(double x0, double x1) {
  SLArCfgAnode::SLArPixIdxCoord pidx = {-9}; 

  int ibin = fAnodeLevelsMap[0]->FindBin(x0, x1); 

  SLArCfgMegaTile* megatile = FindBaseElementInMap(ibin);
  if (megatile) {
    TVector3 mt_pos = TVector3(megatile->GetPhysX(), megatile->GetPhysY(), megatile->GetPhysZ()); 
    //printf("megatile_pos: [%g, %g, %g] mm\n", 
    //mt_pos[0], mt_pos[1], mt_pos[2]); 
 
    pidx[0] = megatile->GetBinIdx();  
    Double_t mt_x0 = mt_pos.Dot(fAxis0); 
    Double_t mt_x1 = mt_pos.Dot(fAxis1); 
    //printf("correct for MT %s coordinates: %g, %g mm -> %g, %g \n", 
        //megatile->GetName(), mt_x0, mt_x1, x0-mt_x0, x1-mt_x1);
    ibin = fAnodeLevelsMap[1]->FindBin(x0-mt_x0, x1-mt_x1);
    SLArCfgReadoutTile* tile = megatile->FindBaseElementInMap(ibin); 
    if (tile) {
      pidx[1] = tile->GetBinIdx(); 
      TVector3 tile_pos(tile->GetPhysX(), tile->GetPhysY(), tile->GetPhysZ()); 
      //printf("tile_pos: [%g, %g, %g]\n", tile_pos[0], tile_pos[1], tile_pos[2]);
      Double_t t_x0 = tile_pos.Dot(fAxis0);
      Double_t t_x1 = tile_pos.Dot(fAxis1); 
      pidx[2] = fAnodeLevelsMap[2]->FindBin(x0-t_x0, x1-t_x1); 
      //printf("pix id %i\n", pidx[2]);
    } 
#ifdef SLAR_DEBUG
    else {
      printf("Cannot find tile with bin index %i\n", ibin);
    }
#endif

  } 
#ifdef SLAR_DEBUG
  else {
    printf("SLArCfgAnode::FindPixel(%g, %g)\n", x0, x1);
    printf("Coordinates outside of anode scope\n"); 
  }
#endif

  return pidx; 
}

void SLArCfgAnode::RegisterMap(size_t ilevel, TH2Poly* hmap) {
  if (fAnodeLevelsMap.count(ilevel)) {
    printf("SLArCfgAnode::RegisterMap(%lu) ERROR\n", ilevel); 
    printf("A map is already registered for level %lu. Quit.\n", ilevel);
    exit(1);
  }

  fAnodeLevelsMap.insert(std::make_pair(ilevel, hmap)); 
  return;
}

TH2Poly* SLArCfgAnode::ConstructPixHistMap(const int depth, 
    const std::vector<int> idx)
{
  switch (depth) {
    // Returns the map of the MegaTiles
    case 0:
      {
        return fAnodeLevelsMap.find(0)->second; 
      }
      break;
    // Returns the map at tile-level for a specfied MTile
    case 1:
      {
        SLArCfgMegaTile* cfgMegaTile  = GetBaseElement(idx[0]); 
        if (!cfgMegaTile) return nullptr; 
        return cfgMegaTile->BuildPolyBinHist(); 
      }
      break;

    // return the map at pixel level for a specified tile
    case 2:
      {
        SLArCfgMegaTile* cfgMegaTile  = GetBaseElement(idx[0]); 
        if (!cfgMegaTile) return nullptr; 
        SLArCfgReadoutTile* cfgTile = cfgMegaTile->GetBaseElement(idx[1]);
        auto tile_pos = 
          TVector3( cfgTile->GetPhysX(), cfgTile->GetPhysY(), cfgTile->GetPhysZ() ); 
        //double tile_xpos = cfgTile->GetPhysZ(); 
        //double tile_ypos = cfgTile->GetPhysY(); 

        TString name_ = Form("hqpix_mt%i_t%i", idx[0], idx[1]); 
        TH2Poly* h2 = new TH2Poly(name_.Data(),
            Form("%s;#it{z} [mm];#it{y} [mm]", name_.Data()),
            (tile_pos-0.5*cfgTile->GetSize()).Dot(fAxis0), 
            (tile_pos+0.5*cfgTile->GetSize()).Dot(fAxis0),
            (tile_pos-0.5*cfgTile->GetSize()).Dot(fAxis1), 
            (tile_pos+0.5*cfgTile->GetSize()).Dot(fAxis1)
            ); 
        h2->SetFloat(); 

        TH2Poly* h2_template = fAnodeLevelsMap.find(2)->second;
        for (const auto& bbin : *(h2_template->GetBins())) {
          TH2PolyBin* bin = (TH2PolyBin*)bbin;
          TGraph* g_tpl = (TGraph*)bin->GetPolygon();
          TGraph* g = (TGraph*)g_tpl->Clone(); 
          for (int i=0; i<g->GetN(); i++) {
            g->GetX()[i] += tile_pos.Dot(fAxis0); 
            g->GetY()[i] += tile_pos.Dot(fAxis1); 
          }
          h2->AddBin(g); 
        }
        return h2;
      }
      break;
  }

  return nullptr;
}

TVector3 SLArCfgAnode::GetPixelCoordinates(const SLArPixIdxCoord& coord) {

#ifdef SLAR_DEBUG
  auto return_null = [coord](const SLArCfgBaseModule* cfg) {
    if (!cfg) {
      printf("SLArCfgAnode::GetPixelCoordinates(%i, %i, %i) ERROR",
          coord.at(0), coord.at(1), coord.at(2));
      printf("cfg not found\n");
    } 
  };
#endif

  
  auto megatile_cfg = fElementsMap.find(coord.at(0))->second;

#ifdef SLAR_DEBUG
  return_null(megatile_cfg);
#endif


  auto tile_cfg = megatile_cfg->GetMap().find(coord.at(1))->second;

#ifdef SLAR_DEBUG
  return_null(tile_cfg);
#endif

  TVector3 xTile( tile_cfg->GetPhysX(), 
                  tile_cfg->GetPhysY(),
                  tile_cfg->GetPhysZ() ); 

  auto pixel_bin = (TH2PolyBin*)fAnodeLevelsMap[2]->GetBins()->At(coord.at(2)-1);
#ifdef SLAR_DEBUG
  printf("Getting bin %i - %i\n", coord.at(2), pixel_bin->GetBinNumber());
#endif

  auto gbin = (TGraph*)pixel_bin->GetPolygon(); 

  double x_pix_local = 0.5*(gbin->GetX()[0] + gbin->GetX()[2]);
  double y_pix_local = 0.5*(gbin->GetY()[0] + gbin->GetY()[2]);

  return xTile + fAxis0*x_pix_local + fAxis1*y_pix_local;
}
