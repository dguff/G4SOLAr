/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgBaseModule.cc
 * @created     : martedì lug 19, 2022 10:54:11 CEST
 */

#include <iostream>
#include <string>
#include <fstream>

#include "TSystemDirectory.h"
#include "TList.h"
#include "TRegexp.h"
#include "TMath.h"

#include "config/SLArCfgBaseModule.hh"

SLArCfgBaseModule::SLArCfgBaseModule() : 
  fIdx(-1), fBin(0), fX(0.), fY(0.), fZ(0.), fPhi(0.), fTheta(0.), fPsi(0.), 
  fPhysX(0.), fPhysY(0.), fPhysZ(0.),  
  fGShape(nullptr)
{}

SLArCfgBaseModule::SLArCfgBaseModule(int idx) : 
  fIdx(idx), fBin(0), fX(0.), fY(0.), fZ(0.), fPhi(0.), fTheta(0.), fPsi(0.), 
  fPhysX(0.), fPhysY(0.), fPhysZ(0.),
  fGShape(nullptr)
{}

SLArCfgBaseModule::SLArCfgBaseModule(int idx, float xc, float yc, float zc, 
             float phi, float theta, float psi) : 
  fIdx(idx), fBin(0), fX(xc), fY(yc), fZ(zc), fPhi(phi), fTheta(theta), fPsi(psi), 
  fPhysX(0.), fPhysY(0.), fPhysZ(0.), 
  fGShape(nullptr)
{
  fIdx   = idx  ; 
  fX     = xc   ;
  fY     = yc   ;
  fZ     = zc   ; 
  fPhi   = phi  ;
  fTheta = theta;
  fPsi   = psi  ;
}

SLArCfgBaseModule::~SLArCfgBaseModule()
{
  if (fGShape) delete fGShape;
}

void SLArCfgBaseModule::SetNormal(double x, double y, double z)
{
  fNormal = TVector3(x, y, z);
}


