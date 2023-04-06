/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArUserTrackInformation.hh
 * @created     Thur Mar 30, 2023 16:48:30 CEST
 */

#include "SLArUserTrackInformation.hh"

SLArUserTrackInformation::SLArUserTrackInformation()
  : G4VUserTrackInformation(), fTrajectory(nullptr), fStoreTrajectory(0)
{}

SLArUserTrackInformation::SLArUserTrackInformation(const G4String& infoType) 
  : G4VUserTrackInformation(infoType), fTrajectory(nullptr), fStoreTrajectory(0)
{}

SLArUserTrackInformation::SLArUserTrackInformation(const SLArUserTrackInformation& info)
  : G4VUserTrackInformation(info) 
{
  fTrajectory = info.fTrajectory; 
  fStoreTrajectory = info.fStoreTrajectory; 
}



