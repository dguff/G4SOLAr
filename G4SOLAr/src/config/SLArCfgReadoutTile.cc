/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgReadoutTile.cc
 * @created     : martedì lug 19, 2022 13:40:08 CEST
 */

#include <iostream>
#include <string>
#include <fstream>
#include "TList.h"

#include "config/SLArCfgReadoutTile.hh"

ClassImp(SLArCfgReadoutTile)

SLArCfgReadoutTile::SLArCfgReadoutTile()
  : SLArCfgBaseModule(), f2DSize_X(0.), f2DSize_Y(0.), fH2Pixels(nullptr)
{}

SLArCfgReadoutTile::SLArCfgReadoutTile(int idx) 
  : SLArCfgBaseModule(idx), f2DSize_X(0.), f2DSize_Y(0.), fH2Pixels(nullptr) 
{}

SLArCfgReadoutTile::SLArCfgReadoutTile(int idx, float xc, float yc, float zc, 
             float phi, float theta, float psi) 
  : SLArCfgBaseModule(idx, xc, yc, zc, phi, theta, psi), 
  f2DSize_X(0.), f2DSize_Y(0.), fH2Pixels(nullptr)
{}

SLArCfgReadoutTile::SLArCfgReadoutTile(const SLArCfgReadoutTile& ref)
  : SLArCfgBaseModule(ref)
{
  f2DSize_X = ref.f2DSize_X; 
  f2DSize_Y = ref.f2DSize_Y; 

  fH2Pixels = nullptr; 
  if (ref.fH2Pixels) {
    fH2Pixels = new TH2Poly(); 
    fH2Pixels->SetNameTitle(ref.fH2Pixels->GetName(), ref.fH2Pixels->GetTitle()); 

    fH2Pixels->SetEntries(ref.fH2Pixels->GetEntries()); 
  }

  for (const auto& gb : ref.fPixelBins) {
    int ibin = fH2Pixels->AddBin(gb.second->Clone()); 
    fPixelBins.insert(std::make_pair(ibin, (TGraph*)gb.second->Clone())); 
  }
}

SLArCfgReadoutTile::~SLArCfgReadoutTile()
{
  if (fGShape) {delete fGShape; fGShape = nullptr;}
  if (fH2Pixels) {delete fH2Pixels; fH2Pixels = nullptr;}
  for (auto &gb : fPixelBins) {
    if (gb.second) {delete gb.second;}
  }
  fPixelBins.clear(); 
}

void SLArCfgReadoutTile::AddPixelToHistMap(std::vector<xypoint> pp)
{
  // check that the first and last point defining the bin are the same 
  if (pp.front() != pp.back()) pp.push_back(pp.front()); 

  TGraph g(pp.size()); 
  int ip = 0; 
  for (const auto &p : pp) {
    //printf("pix[0] = %g, pix[1] = %g\n", p[0], p[1]);
    g.SetPoint(ip, p[0], p[1]); 
    ip++; 
  }

  if (!fH2Pixels) {
    fH2Pixels = new TH2Poly(Form("Tile%i_pixmap", fIdx), 
        Form("Tile %i pixel map", fIdx), -7000, +7000, -3000, 3000);
    fH2Pixels->SetFloat(); 
  }


  int nbin = fH2Pixels->GetNumberOfBins(); 
  int ibin = fH2Pixels->AddBin((TGraph*)g.Clone(Form("pix_%i", nbin))); 

  fPixelBins.insert( std::make_pair(ibin, (TGraph*)g.Clone(Form("pix_%i", nbin))) ); 
}

int SLArCfgReadoutTile::FindPixel(double x, double y) {
  int ibin = fH2Pixels->Fill(x, y); 
  return ibin; 
}

void SLArCfgReadoutTile::BuildGShape() 
{
  fGShape = new TGraph(5);
  fGShape->SetPoint(0, fPhysZ-0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);
  fGShape->SetPoint(1, fPhysZ-0.5*f2DSize_X, fPhysY+0.5*f2DSize_Y);
  fGShape->SetPoint(2, fPhysZ+0.5*f2DSize_X, fPhysY+0.5*f2DSize_Y);
  fGShape->SetPoint(3, fPhysZ+0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);
  fGShape->SetPoint(4, fPhysZ-0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);

  if (fGShape) fGShape->SetName(Form("gShape%i", fIdx)); 
}

void SLArCfgReadoutTile::DumpInfo() 
{
  printf("ReadoutTile id: %i at (%.2f, %.2f) mm, \n", 
      fIdx, fX, fY);
}
