/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgReadoutTile.cc
 * @created     : martedì lug 19, 2022 13:40:08 CEST
 */

#include <iostream>
#include <string>
#include <fstream>

#include "config/SLArCfgReadoutTile.hh"

ClassImp(SLArCfgReadoutTile)

SLArCfgReadoutTile::SLArCfgReadoutTile(): SLArCfgBaseModule(), f2DSize_X(0.), f2DSize_Y(0.)
{}

SLArCfgReadoutTile::SLArCfgReadoutTile(int idx) : SLArCfgBaseModule(idx), f2DSize_X(0.), f2DSize_Y(0.) 
{}

SLArCfgReadoutTile::SLArCfgReadoutTile(int idx, float xc, float yc, float zc, 
             float phi, float theta, float psi) 
  : SLArCfgBaseModule(idx, xc, yc, zc, phi, theta, psi), 
  f2DSize_X(0.), f2DSize_Y(0.)
{}

SLArCfgReadoutTile::~SLArCfgReadoutTile()
{
  if (fGShape) {delete fGShape; fGShape = nullptr;}
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
