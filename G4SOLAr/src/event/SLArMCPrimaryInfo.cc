/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMCPrimaryInfo
 * @created     : venerdì feb 14, 2020 16:56:53 CET
 */

#include <iostream>
#include <string.h>
#include "event/SLArMCPrimaryInfo.hh"

ClassImp(SLArMCPrimaryInfo)

SLArMCPrimaryInfo::SLArMCPrimaryInfo() : 
  fID(0), fName("noParticle"), fEnergy(0.),
  fTotalEdep(0.), fNCherPhotons(0), fNScntPhotons(0), fNWLSPhotons(0),
  fVertex{0}, fMomentum{0}, fEdep3Hist(0) 
{
  fEdep3Hist = new TH3F("hEdep3Hits", "Edep tomography", 
      15,  -750, +750, 
      20, -1000, +1000, 
      15,  -570, +750);
}

SLArMCPrimaryInfo::~SLArMCPrimaryInfo() {
  //std::cerr << "Deleting SLArMCPrimary info (nothing to delete)" << std::endl;
  delete fEdep3Hist;
}

void SLArMCPrimaryInfo::SetPosition(double x, double y,
                                  double z, double t)
{
  fVertex[0] = x;
  fVertex[1] = y;
  fVertex[2] = z;
  fTime   = t;
}

void SLArMCPrimaryInfo::SetMomentum(double px, double py, double pz, 
                                  double ene)
{
  fMomentum[0] = px;
  fMomentum[1] = py;
  fMomentum[2] = pz;
  fEnergy   = ene;
}

void SLArMCPrimaryInfo::ResetParticle()
{
  std::cout << "SLArMCPrimaryInfo::ResetParticle!" << std::endl;
  fID           = 0;
  fName         = "noName";
  fEnergy       = 0.;
  fTime         = 0.;
  fTotalEdep    = 0.;
  fNCherPhotons = 0;
  fNScntPhotons = 0;
  fNWLSPhotons  = 0;
  fEdep3Hist->Reset();

  for (auto &tt : fTrajectories) {delete tt;}
  fTrajectories.clear();
  memset(fVertex  , 0, sizeof(fVertex  ));
  memset(fMomentum, 0, sizeof(fMomentum));
}

void SLArMCPrimaryInfo::PrintParticle()
{
  std::cout << "SLAr Primary Info: " << std::endl;
  std::cout << "Particle:" << fName << " (" << fID <<")" << std::endl;
  std::cout << "Energy  :" << fEnergy <<std::endl;
  std::cout << "Vertex:" << fVertex[0] << ", " 
                         << fVertex[1]<< ", " 
                         << fVertex[2] << " (mm)" << std::endl;
  std::cout << "Momentum:" << fMomentum[0] << ", " 
                           << fMomentum[1]<< ", " 
                           << fMomentum[2]<< std::endl;
}

int SLArMCPrimaryInfo::RegisterTrajectory(SLArEventTrajectory* trj)
{
  fTrajectories.push_back(trj);
  return (int)fTrajectories.size();
}

void SLArMCPrimaryInfo::AddEdep(float x, float y, float z, float edep)
{
  fEdep3Hist->Fill(x, y, z, edep);
  return;
}
