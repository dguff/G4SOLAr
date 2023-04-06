/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetTPC.hh
 * @created     : giovedì nov 03, 2022 12:22:00 CET
 */

#ifndef SLARDETTPC_HH

#define SLARDETTPC_HH

#include "detector/SLArBaseDetModule.hh"
#include "detector/SuperCell/SLArDetSuperCell.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

class SLArDetTPC : public SLArBaseDetModule {

public:
  SLArDetTPC          ();
  virtual ~SLArDetTPC ();

  void          BuildTPC();

  void          BuildMaterial(G4String);
  void          BuildDefalutGeoParMap();
  void          BuildCryostatStructure(const rapidjson::Value& jcryo); 

  const G4ThreeVector GetTPCcenter();
  const G4ThreeVector& GetElectronDriftDir() {return fElectronDriftDir;}
  const G4double& GetElectricField() {return fElectricField;}
  virtual void  Init(const rapidjson::Value& jconf) override; 
  void          SetVisAttributes();


private:
  // Some useful global variables
  SLArMaterial* fMatTarget;
  G4double      fElectricField; 
  G4ThreeVector fElectronDriftDir; 

 
};


#endif /* end of include guard SLARDETTPC_HH */
