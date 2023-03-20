/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetTPC.cc
 * @created     : giovedì nov 03, 2022 12:23:21 CET
 */


#include "SLArDetectorConstruction.hh"
#include "detector/TPC/SLArDetTPC.hh"

#include "SLArAnalysisManager.hh"

#include "G4PhysicalConstants.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Tubs.hh"
#include "G4Ellipsoid.hh"
#include "G4Sphere.hh"
#include "G4Cons.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4VisAttributes.hh"


SLArDetTPC::SLArDetTPC() : SLArBaseDetModule(),
  fMatWorld (nullptr), 
  fMatTarget(nullptr)
{

  fGeoInfo = new SLArGeoInfo();
}


SLArDetTPC::~SLArDetTPC() {
  std::cerr << "Deleting SLArDetTPC..." << std::endl;
  std::cerr << "SLArDetTPC DONE" << std::endl;
}

void SLArDetTPC::BuildMaterial(G4String db_file) 
{
  // TODO: IMPLEMENT PROPER MATERIALS IN /materials
  fMatWorld  = new SLArMaterial();
  fMatTarget = new SLArMaterial();

  fMatTarget->SetMaterialID("LAr");
  fMatTarget->BuildMaterialFromDB(db_file);
}

void SLArDetTPC::BuildDefalutGeoParMap() 
{
  G4cerr << "SLArDetTPC::BuildGeoParMap()" << G4endl;
  fGeoInfo->RegisterGeoPar("target_y"       , 150.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("target_z"       , 200.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("target_x"       ,  60.0*CLHEP::cm); 
  fGeoInfo->RegisterGeoPar("cryo_tk"        ,   1.0*CLHEP::cm);
  G4cerr << "Exit method\n" << G4endl;
}

void SLArDetTPC::BuildTPC() 
{

  G4cerr << "SLArDetTPC::BuildTPC()" << G4endl;
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Building the Target                                 //
  G4cerr << "\tBuilding TPC LAr volume" << G4endl;
  G4double tpcX= fGeoInfo->GetGeoPar("tpc_x");
  G4double tpcY= fGeoInfo->GetGeoPar("tpc_y");
  G4double tpcZ= fGeoInfo->GetGeoPar("tpc_z");
  
  // Create and fill fTarget
  G4double x_ = tpcX*0.5;
  G4double y_ = tpcY*0.5;
  G4double z_ = tpcZ*0.5;

  fModSV = new G4Box("TPC", x_, y_, z_);

  SetLogicVolume(
    new G4LogicalVolume(fModSV, 
      fMatTarget->GetMaterial(),
      "TPC", 0, 0, 0)
    );
}

G4ThreeVector SLArDetTPC::GetTPCcenter() {
  if ( fGeoInfo->Contains("tpc_pos_x") && 
       fGeoInfo->Contains("tpc_pos_y") &&
       fGeoInfo->Contains("tpc_pos_z") ) {
    return G4ThreeVector(fGeoInfo->GetGeoPar("tpc_pos_x"), 
        fGeoInfo->GetGeoPar("tpc_pos_y"), 
        fGeoInfo->GetGeoPar("tpc_pos_z")); 
  }

  return G4ThreeVector(0., 0., 0.); 
}

void SLArDetTPC::SetVisAttributes()
{
  G4cout << "SLArDetTPC::SetVisAttributes()" << G4endl;

  G4VisAttributes* visAttributes = new G4VisAttributes();

  visAttributes->SetVisibility(true); 
  visAttributes->SetColor(0.607, 0.847, 0.992, 0.4);
  fModLV->SetVisAttributes( visAttributes );

  return;
}
