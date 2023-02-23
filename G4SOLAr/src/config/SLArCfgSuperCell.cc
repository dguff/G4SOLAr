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
{}

SLArCfgSuperCell::~SLArCfgSuperCell() 
{
  //if (fGShape) {delete fGShape; fGShape = nullptr;}
}

TGraph* SLArCfgSuperCell::BuildGShape() 
{
  TGraph* g = new TGraph(5);
  g->SetPoint(0, fPhysZ-0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);
  g->SetPoint(1, fPhysZ-0.5*f2DSize_X, fPhysY+0.5*f2DSize_Y);
  g->SetPoint(2, fPhysZ+0.5*f2DSize_X, fPhysY+0.5*f2DSize_Y);
  g->SetPoint(3, fPhysZ+0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);
  g->SetPoint(4, fPhysZ-0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);


  g->SetName(Form("gShape%i", fIdx)); 
  return g; 
}

void SLArCfgSuperCell::DumpInfo() 
{
  printf("SuperCell id: %i at (%.2f, %.2f) mm, \n", 
      fIdx, fX, fY);
}


