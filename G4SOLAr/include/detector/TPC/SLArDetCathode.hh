/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetCathode.hh
 * @created     Mon Mar 20, 2023 16:26:39 CET
 */

#ifndef SLARDETCATHODE_HH

#define SLARDETCATHODE_HH

#include "detector/SLArBaseDetModule.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

class SLArDetCathode : public SLArBaseDetModule {

public:
  SLArDetCathode          ();
  virtual ~SLArDetCathode ();

  void          BuildCathode();

  void          BuildMaterial(G4String);
  void          BuildDefalutGeoParMap();

  G4ThreeVector GetCathodeCenter();
  virtual void  Init(const rapidjson::Value&) override; 
  void          SetVisAttributes();

private:
  // Some useful global variables
  SLArMaterial* fMatCathode;
 
};


#endif /* end of include guard SLARDETCATHODE_HH */
