/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArEvTrajectory
 * @created     : lunedì ago 31, 2020 17:39:33 CEST
 */

#include "event/SLArEventTrajectory.hh"

ClassImp(SLArEventTrajectory)

SLArEventTrajectory::SLArEventTrajectory() : 
  fParticleName("noName"), fPDGID(0), fTrackID(-1), fParentID(-1), 
  fInitKineticEnergy(0.), fTrackLength(0.), fInitMomentum(TVector3(0,0,0))
{}

SLArEventTrajectory::~SLArEventTrajectory()
{
  fParticleName = "noName";
  fTrackID      = -1;
  fParentID     = -1;
  fInitKineticEnergy = 0.;
  fTrackLength  = 0;
  fInitMomentum      = TVector3(0,0,0);
  fTrjPoints.clear();
}

void SLArEventTrajectory::RegisterPoint(double x, double y, double z, double edep)
{
  fTrjPoints.push_back( trj_point(x, y, z, edep) );
  return;
}

float SLArEventTrajectory::GetTotalEdep() { 
  float edep = 0.; 
  for (const auto &pt : fTrjPoints) {
    edep += pt.fEdep; 
  }             
  return edep; 
}
