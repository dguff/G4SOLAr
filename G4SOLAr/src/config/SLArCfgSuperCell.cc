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

SLArCfgSuperCell::SLArCfgSuperCell() : SLArCfgBaseModule()
{}

SLArCfgSuperCell::SLArCfgSuperCell(int idx) : SLArCfgBaseModule(idx)
{}

SLArCfgSuperCell::SLArCfgSuperCell(int idx, float xc, float yc, float zc, 
             float phi, float theta, float psi) 
  : SLArCfgBaseModule(idx, xc, yc, zc, phi, theta, psi)
{}

SLArCfgSuperCell::~SLArCfgSuperCell() 
{
  //if (fGShape) {delete fGShape; fGShape = nullptr;}
}

TGraph SLArCfgSuperCell::BuildGShape() 
{
  TGraph g(5);
  TVector3 pos(fPhysX, fPhysY, fPhysZ); 
  TVector3 size_tmp = fSize; 
  TRotation rot; 
  rot.SetXPhi( fPhi ); rot.SetXTheta( fTheta ); rot.SetXPsi( fPsi ); 
  rot = rot.Inverse(); 
  size_tmp.Transform( rot ); 
  
  g.SetPoint(0, fAxis0.Dot(pos-0.5*size_tmp), fAxis1.Dot(pos-0.5*size_tmp));
  g.SetPoint(1, fAxis0.Dot(pos-0.5*size_tmp), fAxis1.Dot(pos+0.5*size_tmp));
  g.SetPoint(2, fAxis0.Dot(pos+0.5*size_tmp), fAxis1.Dot(pos+0.5*size_tmp));
  g.SetPoint(3, fAxis0.Dot(pos+0.5*size_tmp), fAxis1.Dot(pos-0.5*size_tmp));
  g.SetPoint(4, fAxis0.Dot(pos-0.5*size_tmp), fAxis1.Dot(pos-0.5*size_tmp));

  //printf("gbin:"); 
  //for (int i=0; i<5; i++) {
    //printf(" - [%g, %g]", g->GetX()[i], g->GetY()[i]); 
  //}
  //printf("\n");

  //g->SetPoint(0, fPhysZ-0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);
  //g->SetPoint(1, fPhysZ-0.5*f2DSize_X, fPhysY+0.5*f2DSize_Y);
  //g->SetPoint(2, fPhysZ+0.5*f2DSize_X, fPhysY+0.5*f2DSize_Y);
  //g->SetPoint(3, fPhysZ+0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);
  //g->SetPoint(4, fPhysZ-0.5*f2DSize_X, fPhysY-0.5*f2DSize_Y);


  g.SetName(Form("gShape%i", fIdx)); 
  return g; 
}

void SLArCfgSuperCell::DumpInfo() 
{
  printf("SuperCell id: %i at (%.2f, %.2f) mm, \n", 
      fIdx, fX, fY);
}


