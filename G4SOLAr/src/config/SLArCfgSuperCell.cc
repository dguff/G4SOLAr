/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArCfgSuperCell
 * @created     : lunedì feb 10, 2020 18:52:43 CET
 */

#include <iostream>
#include <string>
#include <fstream>

#include "TSystemDirectory.h"
#include "TList.h"
#include "TRegexp.h"
#include "TMath.h"

#include "config/SLArCfgSuperCell.hh"

SLArCfgSuperCell::SLArCfgSuperCell() : 
  fIdx(-1), fBin(0), fX(0.), fY(0.), fZ(0.), fPhi(0.), fTheta(0.), fPsi(0.), 
  fPhysX(0.), fPhysY(0.), fPhysZ(0.),  
  fGShape(nullptr), fModel("")
{}

SLArCfgSuperCell::SLArCfgSuperCell(int idx) : 
  fIdx(idx), fBin(0), fX(0.), fY(0.), fZ(0.), fPhi(0.), fTheta(0.), fPsi(0.), 
  fPhysX(0.), fPhysY(0.), fPhysZ(0.),
  fGShape(nullptr), fModel("")
{}

SLArCfgSuperCell::SLArCfgSuperCell(int idx, float xc, float yc, float zc, 
             float phi, float theta, float psi)
{
  fIdx   = idx  ; 
  fX     = xc   ;
  fY     = yc   ;
  fZ     = zc   ; 
  fPhi   = phi  ;
  fTheta = theta;
  fPsi   = psi  ;
  //
  BuildGShape();
}

SLArCfgSuperCell::~SLArCfgSuperCell()
{
  //std::cerr << "Deleting SLArCfgSuperCell..." << std::endl;
  if (fGShape) delete fGShape;
  //for (auto &itr : fHits)
  //{
    //if (itr) delete itr;
  //}
  //fHits.clear();
  //std::cerr << "SLArCfgSuperCell DONE" << std::endl;
}

void SLArCfgSuperCell::BuildGShape() 
{
  fGShape = new TGraph();
  fGShape->SetPoint(0, fX-0.5*f2DSize_X, fY-0.5*f2DSize_Y);
  fGShape->SetPoint(1, fX-0.5*f2DSize_X, fY+0.5*f2DSize_Y);
  fGShape->SetPoint(2, fX+0.5*f2DSize_X, fY+0.5*f2DSize_Y);
  fGShape->SetPoint(3, fX+0.5*f2DSize_X, fY-0.5*f2DSize_Y);
  fGShape->SetPoint(4, fX-0.5*f2DSize_X, fY-0.5*f2DSize_Y);


  if (fGShape) fGShape->SetName(Form("gShape%i", fIdx)); 
}

//void SLArCfgSuperCell::RecordHit(BCEventHitPMT* hit)
//{
  //fHits.push_back(hit);
//}

//std::vector<BCEventHitPMT*>& SLArCfgSuperCell::GetHits()
//{
  //std::sort(fHits.begin(), fHits.end(), 
            //BCEventHitPMT::CompareHitPtrs);
  //return fHits;
//}

//bool SLArCfgSuperCell::IsHit() 
//{
  //return fHits.size();
//}

//double SLArCfgSuperCell::GetTime(){
  //double t = 0;
  //if (!IsHit()) return t;
  //else {
    //std::sort(fHits.begin(), fHits.end());
    //return fHits.at(0)->GetTime();
  //}
//}

//double SLArCfgSuperCell::GetCharge()
//{
  //double chg = 0;
  //if (!IsHit()) return chg;
  //else {
    //for (auto &hit : fHits)
      //chg += hit->GetCharge();
    //return chg;
  //}
//}

//void SLArCfgSuperCell::ClearHits() {
  //fHits.clear();
//}

void SLArCfgSuperCell::DumpInfo() 
{
  printf("SuperCell id: %i at (%.2f, %.2f) mm, \n", 
      fIdx, fX, fY);
}



void SLArCfgSuperCell::SetPMTNormal(double x, double y, double z)
{
  fNormal = TVector3(x, y, z);
}


