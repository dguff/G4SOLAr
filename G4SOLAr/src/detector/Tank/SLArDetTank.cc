/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArDetTank
 * @created     : mercoledì lug 31, 2019 16:02:13 CEST
 */

#include "SLArDetectorConstruction.hh"
#include "detector/Tank/SLArDetTank.hh"

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

SLArDetTank::SLArDetTank() : SLArBaseDetModule(),
  fTarget   (nullptr), fVessel (nullptr)  ,  
  fBoxOut   (nullptr), fBoxInn (nullptr)  ,
  fMatWorld (nullptr), fMatVessel(nullptr)
{

  fGeoInfo = new SLArGeoInfo();
}


SLArDetTank::~SLArDetTank() {
  std::cerr << "Deleting SLArDetTank..." << std::endl;
  if (fTarget   ) {delete fTarget   ; fTarget    = NULL;}
  if (fVessel   ) {delete fVessel   ; fVessel    = NULL;}
  if (fWindow   ) {delete fWindow   ; fWindow    = NULL;}
  if (fBoxOut   ) {delete fBoxOut   ; fBoxOut    = NULL;}
  if (fBoxInn   ) {delete fBoxInn   ; fBoxInn    = NULL;}
  std::cerr << "SLArDetTank DONE" << std::endl;
}

void SLArDetTank::BuildMaterial() 
{
  // TODO: IMPLEMENT PROPER MATERIALS IN /materials
  fMatWorld  = new SLArMaterialInfo();
  fMatVessel = new SLArMaterialInfo();
  fMatTarget = new SLArMaterialInfo();

  fMatVessel->SetMaterialID("Steel");
  fMatVessel->GetMaterialBuilder()->BuildMaterial();

  fMatTarget->SetMaterialID("LAr");
  fMatTarget->GetMaterialBuilder()->BuildMaterial();

  fMatWorld ->SetMaterialID("Air");
  fMatWorld ->GetMaterialBuilder()->BuildMaterial();
}

void SLArDetTank::BuildDefalutGeoParMap() 
{
  G4cerr << "SLArDetTank::BuildGeoParMap()" << G4endl;
  fGeoInfo->RegisterGeoPar("target_y"       , 150.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("target_z"       , 200.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("target_x"       ,  60.0*CLHEP::cm); 
  fGeoInfo->RegisterGeoPar("cryo_tk"        ,   1.0*CLHEP::cm);
  G4cerr << "Exit method\n" << G4endl;
}

void SLArDetTank::BuildVessel()
{
  G4cerr << "SLArDetTank::BuildVessel()" << G4endl;
  G4double tgtZ         = fGeoInfo->GetGeoPar("target_z");
  G4double tgtY         = fGeoInfo->GetGeoPar("target_y");
  G4double tgtX         = fGeoInfo->GetGeoPar("target_x");
  G4double tnkThck      = fGeoInfo->GetGeoPar("cryo_tk" );
  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Create Tank 
  G4double x_ = tgtX*0.5 + tnkThck;
  G4double y_ = tgtY*0.5 + tnkThck;
  G4double z_ = tgtZ*0.5 + tnkThck;
  // Create outer box 
  fBoxOut = new G4Box("fBoxOut_solid", 
      x_, y_, z_); 

  // Create inner box 
  fBoxInn = new G4Box("fBoxInn_solid", 
      x_-tnkThck, y_-tnkThck, z_-tnkThck);

  G4SubtractionSolid* vessel_solid = 
    new G4SubtractionSolid("vessel_solid", 
        fBoxOut, fBoxInn, 0, G4ThreeVector(0,0,0));

  // Create Vessel
  G4cerr << "Create Vessel" << G4endl;
  fVessel = new SLArBaseDetModule();
  fVessel->SetGeoPar("vessel_x", 2*x_);
  fVessel->SetGeoPar("vessel_y", 2*y_);
  fVessel->SetGeoPar("vessel_z", 2*z_);
  fVessel->SetMaterial(fMatVessel->GetMaterial());
  fVessel->SetSolidVolume(vessel_solid);

  fVessel->SetLogicVolume(
    new G4LogicalVolume(fVessel->GetModSV(), 
      fVessel->GetMaterial(),
      "VesselLV", 0, 0, 0)
    );
}


void SLArDetTank::BuildTarget()
{
  G4cerr << "SLArDetTank::BuildTarget()" << G4endl;
  G4double tgtX= fGeoInfo->GetGeoPar("target_x");
  G4double tgtY= fGeoInfo->GetGeoPar("target_y");
  G4double tgtZ= fGeoInfo->GetGeoPar("target_z");
  
  // Create and fill fTarget
  G4cerr << "Create and fill fTarget" << G4endl;
  G4double x_ = tgtX*0.5;
  G4double y_ = tgtY*0.5;
  G4double z_ = tgtZ*0.5;

  fTarget = new SLArBaseDetModule();
  fTarget->SetGeoPar(fGeoInfo->GetGeoPair("target_x"));
  fTarget->SetGeoPar(fGeoInfo->GetGeoPair("target_y"));
  fTarget->SetGeoPar(fGeoInfo->GetGeoPair("target_z"));
  fTarget->SetMaterial(fMatTarget->GetMaterial());
  fTarget->SetSolidVolume(new G4Box("Target", x_, y_, z_));

  fTarget->SetLogicVolume(
    new G4LogicalVolume(fTarget->GetModSV(), 
      fTarget->GetMaterial(),
      "TargetLV", 0, 0, 0)
    );
}

void SLArDetTank::BuildTank() 
{

  G4cerr << "SLArDetTank::BuildTank()" << G4endl;
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Building the Target                                 //
  G4cerr << "\tBuilding Vessel, Window and Target" << G4endl;
  BuildVessel();
  BuildTarget();
  
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Building a "empty" LV as Tank container             //
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  fModLV
    = new G4LogicalVolume(fBoxOut,
        fMatWorld->GetMaterial(),
        "TankLV",0,0,0);
 
  //* * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Place Steel Tank
  G4cerr << "\tPlacing Vessel" << G4endl;

  fVessel->GetModPV("Vessel", 0, G4ThreeVector(0, 0, 0), 
      fModLV, false, 24);


  //* * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Place LAr
  G4cerr << "\tPlacing Target" << G4endl;

  fTarget->GetModPV("Target", 0, 
      G4ThreeVector(0, 0, 0), 
      fModLV, false, 24);

  G4cerr << "end of method\n" << G4endl;
}


void SLArDetTank::ResetTankGeometry() 
{
  G4cout<< "Reset Tank Geometry" << G4endl;
  
  fTarget->ResetGeometry();
  fVessel->ResetGeometry();

  return; 
}

void SLArDetTank::SetVisAttributes()
{
  G4cout << "SLArDetTank::SetVisAttributes()" << G4endl;

  G4VisAttributes* visAttributes = new G4VisAttributes();

  visAttributes = new G4VisAttributes();
  visAttributes->SetColour(0.611, 0.847, 0.988, 0.6);
  if (fVessel)
    fVessel->GetModLV()->SetVisAttributes( visAttributes );

  visAttributes = new G4VisAttributes();
  visAttributes->SetColor(0.607, 0.847, 0.992, 0.4);
  if (fTarget)
    fTarget->GetModLV()->SetVisAttributes(visAttributes);

  return;
}
