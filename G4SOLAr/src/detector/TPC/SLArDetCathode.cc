/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetCathode.cc
 * @created     Mon Mar 20, 2023 16:28:35 CET
 */


#include "SLArDetectorConstruction.hh"
#include "detector/TPC/SLArDetCathode.hh"

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


SLArDetCathode::SLArDetCathode() : SLArBaseDetModule(),
  fMatCathode(nullptr)
{
  fGeoInfo = new SLArGeoInfo();
}


SLArDetCathode::~SLArDetCathode() {
  std::cerr << "Deleting SLArDetCathode..." << std::endl;
  std::cerr << "SLArDetCathode DONE" << std::endl;
}

void SLArDetCathode::BuildMaterial(G4String db_file) 
{
  // TODO: IMPLEMENT PROPER MATERIALS IN /materials
  fMatCathode = new SLArMaterial();

  fMatCathode->SetMaterialID("FR4");
  fMatCathode->BuildMaterialFromDB(db_file);
}

void SLArDetCathode::BuildDefalutGeoParMap() 
{
  G4cerr << "SLArDetCathode::BuildGeoParMap()" << G4endl;
  fGeoInfo->RegisterGeoPar("pos_x"       ,   0.0*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("pos_y"       ,   0.0*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("pos_z"       ,   0.0*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("dim_y"       , 150.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("dim_z"       , 200.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("dim_x"       ,  60.0*CLHEP::cm); 
  G4cerr << "Exit method\n" << G4endl;
}

void SLArDetCathode::BuildCathode() 
{

  G4cerr << "SLArDetCathode::BuildCathode()" << G4endl;
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Building the Target                                 //
  G4cerr << "\tBuilding TPC Cathode" << G4endl;
  G4double cathX= fGeoInfo->GetGeoPar("dim_x");
  G4double cathY= fGeoInfo->GetGeoPar("dim_y");
  G4double cathZ= fGeoInfo->GetGeoPar("dim_z");
  
  // Create and fill fTarget
  G4double x_ = cathX*0.5;
  G4double y_ = cathY*0.5;
  G4double z_ = cathZ*0.5;

  fModSV = new G4Box("cathode"+std::to_string(fID)+"_sv", x_, y_, z_);

  SetLogicVolume(
    new G4LogicalVolume(fModSV, 
      fMatCathode->GetMaterial(),
      "cathode"+std::to_string(fID)+"_lv", 0, 0, 0)
    );
 }

G4ThreeVector SLArDetCathode::GetCathodeCenter() {
  if ( fGeoInfo->Contains("pos_x") && 
       fGeoInfo->Contains("pos_y") &&
       fGeoInfo->Contains("pos_z") ) {
    return G4ThreeVector(fGeoInfo->GetGeoPar("pos_x"), 
        fGeoInfo->GetGeoPar("pos_y"), 
        fGeoInfo->GetGeoPar("pos_z")); 
  }

  return G4ThreeVector(0., 0., 0.); 
}

void SLArDetCathode::SetVisAttributes()
{
  G4cout << "SLArDetCathode::SetVisAttributes()" << G4endl;

  G4VisAttributes* visAttributes = new G4VisAttributes();

  visAttributes->SetVisibility(true); 
  visAttributes->SetColor(0.607, 0.847, 0.992, 0.4);
  fModLV->SetVisAttributes( visAttributes );

  return;
}

void SLArDetCathode::Init(const rapidjson::Value& jconf) {
  assert(jconf.IsObject()); 
  auto jcathode = jconf.GetObject(); 

  assert(jcathode.HasMember("dimensions")); 
  assert(jcathode.HasMember("position")); 
  assert(jcathode.HasMember("copyID")); 

  jcathode["copyID"].GetInt(); 
  fGeoInfo->ReadFromJSON(jcathode["position"].GetObj(), "pos"); 
  fGeoInfo->ReadFromJSON(jcathode["dimensions"].GetArray()); 

  return;
}
