/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgBaseModule.cc
 * @created     : martedì lug 19, 2022 10:54:11 CEST
 */

#include "config/SLArCfgBaseModule.hh"

ClassImp(SLArCfgBaseModule)

SLArCfgBaseModule::SLArCfgBaseModule() : TNamed(), 
  fIdx(-1), fBin(0), fX(0.), fY(0.), fZ(0.), fPhi(0.), fTheta(0.), fPsi(0.), 
  fPhysX(0.), fPhysY(0.), fPhysZ(0.), fNormal(0, 0, 1) 
  //fGShape(nullptr)
{}

SLArCfgBaseModule::SLArCfgBaseModule(int idx) : TNamed(),
  fIdx(idx), fBin(0), fX(0.), fY(0.), fZ(0.), fPhi(0.), fTheta(0.), fPsi(0.), 
  fPhysX(0.), fPhysY(0.), fPhysZ(0.), fNormal(0, 0, 1)
  //fGShape(nullptr)
{}

SLArCfgBaseModule::SLArCfgBaseModule(int idx, float xc, float yc, float zc, 
             float phi, float theta, float psi) : TNamed(), 
  fIdx(idx), fBin(0), fX(xc), fY(yc), fZ(zc), fPhi(phi), fTheta(theta), fPsi(psi), 
  fPhysX(0.), fPhysY(0.), fPhysZ(0.), fNormal(0, 0, 1)
  //fGShape(nullptr)
{
  fIdx   = idx  ; 
  fX     = xc   ;
  fY     = yc   ;
  fZ     = zc   ; 
  fPhi   = phi  ;
  fTheta = theta;
  fPsi   = psi  ;
}

SLArCfgBaseModule::SLArCfgBaseModule(const SLArCfgBaseModule& base) : 
  SLArCfgBaseModule() 
{
    fIdx = base.fIdx;
    fBin = base.fBin;
    fX = base.fX;
    fY = base.fY;
    fZ = base.fZ;
    fPhi = base.fPhi;
    fTheta = base.fTheta;
    fPsi = base.fPsi;
    fPhysX = base.fPhysX;
    fPhysY = base.fPhysY;
    fPhysZ = base.fPhysZ;
    fNormal = base.fNormal;
    //if (base.fGShape) {
      //fGShape = (TGraph*)base.fGShape->Clone();
    //}
}

SLArCfgBaseModule::~SLArCfgBaseModule()
{
  //if (fGShape) delete fGShape;
}


