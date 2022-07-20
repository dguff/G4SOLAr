/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgSuperCell.cc
 * @created     : martedì lug 19, 2022 10:59:27 CEST
 */

#include <iostream>
#include <string>
#include <fstream>

#include "config/SLArCfgSuperCell.hh"

ClassImp(SLArCfgSuperCell)

SLArCfgSuperCell::SLArCfgSuperCell() : SLArCfgBaseModule(), f2DSize_X(0.), f2DSize_Y(0.)
{}

SLArCfgSuperCell::SLArCfgSuperCell(int idx) : SLArCfgBaseModule(idx), f2DSize_X(0.), f2DSize_Y(0.)
{}

SLArCfgSuperCell::SLArCfgSuperCell(int idx, float xc, float yc, float zc, 
             float phi, float theta, float psi) 
  : SLArCfgBaseModule(idx, xc, yc, zc, phi, theta, psi), 
  f2DSize_X(0.), f2DSize_Y(0.)
{

  BuildGShape();
}

SLArCfgSuperCell::~SLArCfgSuperCell() 
{
  if (fGShape) {delete fGShape; fGShape = nullptr;}
}

void SLArCfgSuperCell::BuildGShape() 
{
  fGShape = new TGraph(5);
  fGShape->SetPoint(0, fX-0.5*f2DSize_X, fY-0.5*f2DSize_Y);
  fGShape->SetPoint(1, fX-0.5*f2DSize_X, fY+0.5*f2DSize_Y);
  fGShape->SetPoint(2, fX+0.5*f2DSize_X, fY+0.5*f2DSize_Y);
  fGShape->SetPoint(3, fX+0.5*f2DSize_X, fY-0.5*f2DSize_Y);
  fGShape->SetPoint(4, fX-0.5*f2DSize_X, fY-0.5*f2DSize_Y);


  if (fGShape) fGShape->SetName(Form("gShape%i", fIdx)); 
}

void SLArCfgSuperCell::DumpInfo() 
{
  printf("SuperCell id: %i at (%.2f, %.2f) mm, \n", 
      fIdx, fX, fY);
}


