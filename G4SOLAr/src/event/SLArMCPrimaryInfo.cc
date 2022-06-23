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
  fID(0), fTrkID(0), fName("noParticle"), fEnergy(0.),
  fTotalEdep(0.),
  fVertex{0}, fMomentum{0}
{}

SLArMCPrimaryInfo::~SLArMCPrimaryInfo() {}

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
  fID           = 0;
  fTrkID        = 0; 
  fName         = "noName";
  fEnergy       = 0.;
  fTime         = 0.;
  fTotalEdep    = 0.;

  for (auto &tt : fTrajectories) {delete tt;}
  fTrajectories.clear();
  memset(fVertex  , 0, sizeof(fVertex  ));
  memset(fMomentum, 0, sizeof(fMomentum));
}

void SLArMCPrimaryInfo::PrintParticle()
{
  std::cout << "SLAr Primary Info: " << std::endl;
  std::cout << "Particle:" << fName << "id: " << fID <<", trk id: " << fTrkID << std::endl;
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


