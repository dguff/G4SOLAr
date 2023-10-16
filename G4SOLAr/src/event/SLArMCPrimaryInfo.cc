/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArMCPrimaryInfo.cc
 * @created     Fri Feb 14, 2020 16:56:53 CET
 */


#include <iostream>
#include <string.h>
#include <memory>
#include "event/SLArMCPrimaryInfo.hh"

ClassImp(SLArMCPrimaryInfo)

SLArMCPrimaryInfo::SLArMCPrimaryInfo() : 
  fID(0), fTrkID(0), fName("noParticle"), fEnergy(0.),
  fTotalEdep(0.), fTotalLArEdep(0), fTotalScintPhotons(0), fTotalCerenkovPhotons(0),
  fVertex(3, 0.), fMomentum(3, 0.)
{
  fTrajectories.reserve(100);
}

SLArMCPrimaryInfo::SLArMCPrimaryInfo(const SLArMCPrimaryInfo& p) 
  : TNamed(p), 
    fID(p.fID), fTrkID(p.fTrkID), fEnergy(p.fEnergy), 
    fTotalEdep(p.fTotalEdep), fTotalLArEdep(p.fTotalLArEdep), 
    fTotalScintPhotons(p.fTotalScintPhotons), fTotalCerenkovPhotons(p.fTotalCerenkovPhotons),
    fVertex(p.fVertex), fMomentum(p.fMomentum) 
{
  for (const auto& t : p.fTrajectories) {
    fTrajectories.push_back( std::make_unique<SLArEventTrajectory>(*t) ); 
  }
}

SLArMCPrimaryInfo::~SLArMCPrimaryInfo() {
  fTrajectories.clear();
}

void SLArMCPrimaryInfo::SetPosition(const double& x, const double& y,
                                    const double& z, const double& t)
{
  fVertex[0] = x;
  fVertex[1] = y;
  fVertex[2] = z;
  fTime   = t;
  fTotalCerenkovPhotons = 0; 
  fTotalScintPhotons = 0; 
}

void SLArMCPrimaryInfo::SetMomentum(const double& px, const double& py, const double& pz, const double& ene)
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
  fTotalLArEdep = 0.;
  fTotalScintPhotons = 0; 
  fTotalCerenkovPhotons = 0; 

  //for (auto &t :fTrajectories) {
    //delete t;
  //}
  fTrajectories.clear();
  std::fill(fVertex.begin(), fVertex.end(), 0.); 
  std::fill(fMomentum.begin(), fMomentum.end(), 0.); 
}


void SLArMCPrimaryInfo::PrintParticle() const
{
  std::cout << "SLAr Primary Info: " << std::endl;
  std::cout << "Particle:" << fName << ", id: " << fID <<", trk id: " << fTrkID << std::endl;
  std::cout << "Energy  :" << fEnergy <<std::endl;
  std::cout << "Vertex:" << fVertex[0] << ", " 
                         << fVertex[1]<< ", " 
                         << fVertex[2] << " (mm)" << std::endl;
  std::cout << "Momentum:" << fMomentum[0] << ", " 
                           << fMomentum[1]<< ", " 
                           << fMomentum[2]<< std::endl;
}

int SLArMCPrimaryInfo::RegisterTrajectory(std::unique_ptr<SLArEventTrajectory> trj)
{
  fTotalEdep += trj->GetTotalEdep(); 
  //fTrajectories.push_back(trj);
  fTrajectories.push_back( std::move(trj) );
  //printf("Added trj %i to primary register\n", trj->GetTrackID());
  return (int)fTrajectories.size();
}

//template<>
//int SLArMCPrimaryInfoPtr::RegisterTrajectory(SLArEventTrajectory* trj)
//{
  //fTotalEdep += trj->GetTotalEdep(); 
  //fTrajectories.push_back( std::move(trj) );
  ////printf("Added trj %i to primary register\n", trj->GetTrackID());
  //return (int)fTrajectories.size();
//}



