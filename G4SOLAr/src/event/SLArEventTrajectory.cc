/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArEvTrajectory.cc
 * @created     Mon Aug 31, 2020 17:39:33 CEST
 */

#include "event/SLArEventTrajectory.hh"
#include <cstdio>

ClassImp(SLArEventTrajectory)

SLArEventTrajectory::SLArEventTrajectory() : 
  fParticleName("noName"), 
  fCreatorProcess("noCreator"), 
  fEndProcess("noDestroyer"),
  fPDGID(0), fTrackID(-1), fParentID(-1), 
  fInitKineticEnergy(0.), fTrackLength(0.), fTime(0.), fWeight(1.),
  fInitMomentum(TVector3(0,0,0)), 
  fTotalEdep(0.), fTotalNph(0.), fTotalNel(0.)
{
  //fTrjPoints.reserve(500);
}

SLArEventTrajectory::SLArEventTrajectory(const SLArEventTrajectory& trj) 
  : TObject(trj)
{
  //printf("Creating new SLArEventTrajectory with copy costructor\n");
  //printf("trk ID %i, PDG ID %i - trj size %lu\n", 
      //trj.GetTrackID(), 
      //trj.GetPDGID(), 
      //trj.fTrjPoints.size());

  fParticleName = trj.fParticleName; 
  fCreatorProcess = trj.fCreatorProcess; 
  fEndProcess = trj.fEndProcess; 
  fPDGID = trj.fPDGID; 
  fTrackID = trj.fTrackID; 
  fParentID = trj.fParentID; 
  fInitKineticEnergy = trj.fInitKineticEnergy; 
  fTrackLength = trj.fTrackLength; 
  fTime = trj.fTime; 
  fWeight = trj.fWeight;
  fInitMomentum = trj.fInitMomentum; 
  fTotalEdep = trj.fTotalEdep; 
  fTotalNph = trj.fTotalNph; 
  fTotalNel = trj.fTotalNel; 

  for (const trj_point& pt : trj.fTrjPoints) {
    fTrjPoints.push_back( pt );
  }
  
}

SLArEventTrajectory::~SLArEventTrajectory()
{
  fParticleName = "noName";
  fTrackID      = -1;
  fParentID     = -1;
  fInitKineticEnergy = 0.;
  fTrackLength  = 0;
  fInitMomentum = TVector3(0,0,0);
  fTrjPoints.clear();
}

void SLArEventTrajectory::RegisterPoint(const trj_point& point) {
  fTrjPoints.push_back( point ); 
  return; 
}

void SLArEventTrajectory::RegisterPoint(double x, double y, double z, double energy, double edep, int n_ph, int n_el, int copy)
{
  fTrjPoints.push_back( trj_point(x, y, z, energy, edep, n_ph, n_el, copy) );
  return;
}


