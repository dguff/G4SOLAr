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
  fMatTarget = new SLArMaterial();

  fMatTarget->SetMaterialID("LAr");
  fMatTarget->BuildMaterialFromDB(db_file);
}

void SLArDetTPC::BuildDefalutGeoParMap() 
{
  G4cerr << "SLArDetTPC::BuildGeoParMap()" << G4endl;
  fGeoInfo->RegisterGeoPar("tpc_y"       , 150.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("tpc_z"       , 200.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("tpc_x"       ,  60.0*CLHEP::cm); 
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

const G4ThreeVector SLArDetTPC::GetTPCcenter() {
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

void SLArDetTPC::Init(const rapidjson::Value& jconf) {
  assert(jconf.IsObject()); 
  auto jtpc = jconf.GetObject(); 
  assert(jtpc.HasMember("dimensions")); 
  assert(jtpc.HasMember("position"  )); 
  assert(jtpc.HasMember("copyID"    )); 

  SetID(jtpc["copyID"].GetInt()); 

  fGeoInfo->ReadFromJSON(jtpc["dimensions"].GetArray()); 

  const auto jposition = jtpc["position"].GetObj(); 
  G4double vunit = 1.0; 
  if (jposition.HasMember("unit")) {
    vunit = G4UIcommand::ValueOf(jposition["unit"].GetString());
  }
  G4String xvar[3] = {"x", "y", "z"}; 
  assert(jposition.HasMember("xyz")); 
  int ii=0; 
  for (const auto &v : jposition["xyz"].GetArray()) {
    G4double tmp = v.GetDouble() * vunit; 
    fGeoInfo->RegisterGeoPar("tpc_pos_"+xvar[ii], tmp); 
    ++ii; 
  }

  if (jtpc.HasMember("electric_field")) {
    fElectricField = SLArGeoInfo::ParseJsonVal( jtpc["electric_field"] ); 
    printf("electric_field is %g kV/cm\n", fElectricField/(CLHEP::kilovolt/CLHEP::cm)); 
    auto jfield = jtpc["electric_field"].GetObj(); 
    auto jdir   = jfield["direction"].GetArray(); 
    assert(jdir.Size() == 3); 
    int idim = 0; 
    for (const auto &v : jdir) {
      fElectronDriftDir[idim] = v.GetDouble();
      idim++; 
    }
  } 
  else {
    printf("SLArDetTPC::Init WARNING: Electric field properties are not specified. "); 
    printf("Setting default values: E = 0.5 kV/cm, drift dir (1, 0, 0)\n"); 
    fElectricField = 0.5*(CLHEP::kilovolt/CLHEP::cm); 
    fElectronDriftDir = G4ThreeVector(1, 0, 0); 
  } 

}
