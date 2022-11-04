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

SLArCryostatLayer::SLArCryostatLayer() : 
  fName("CryostatLayer"), 
  fHalfSizeX(1.), fHalfSizeY(1.), fHalfSizeZ(1.), fThickness(1.), 
  fMaterialName("MATERIAL_NAME"), fMaterial(nullptr), fModule(nullptr)
{}

SLArCryostatLayer::SLArCryostatLayer(
    G4String   model_name, 
    G4double*  halfSize,  
    G4double   thickness,
    G4String   material_name)
  : fMaterial(nullptr), fModule(nullptr)
{

  fName      = model_name;
  fHalfSizeX =  halfSize[0];
  fHalfSizeY =  halfSize[1]; 
  fHalfSizeZ =  halfSize[2]; 
  fThickness =  thickness; 

  fMaterialName = material_name;
}

SLArCryostatLayer::~SLArCryostatLayer() {}

SLArDetTPC::SLArDetTPC() : SLArBaseDetModule(),
  fTarget   (nullptr), fCryostat (nullptr)  ,  
  fBoxOut   (nullptr), fBoxInn (nullptr)  ,
  fMatWorld (nullptr), fMatSteel(nullptr) , 
  fMatPlywood(nullptr), fMatPolyurethane(nullptr), 
  fMatBPolyethilene(nullptr), fMatTarget(nullptr)
{

  fGeoInfo = new SLArGeoInfo();
}


SLArDetTPC::~SLArDetTPC() {
  std::cerr << "Deleting SLArDetTPC..." << std::endl;
  if (fTarget   ) {delete fTarget   ; fTarget    = NULL;}
  if (fCryostat ) {delete fCryostat ; fCryostat  = NULL;}
  if (fWindow   ) {delete fWindow   ; fWindow    = NULL;}
  if (fBoxOut   ) {delete fBoxOut   ; fBoxOut    = NULL;}
  if (fBoxInn   ) {delete fBoxInn   ; fBoxInn    = NULL;}
  std::cerr << "SLArDetTPC DONE" << std::endl;
}

void SLArDetTPC::BuildMaterial(G4String db_file) 
{
  // TODO: IMPLEMENT PROPER MATERIALS IN /materials
  fMatWorld  = new SLArMaterial();
  fMatSteel  = new SLArMaterial();
  fMatPlywood = new SLArMaterial(); 
  fMatPolyurethane = new SLArMaterial(); 
  fMatBPolyethilene = new SLArMaterial(); 
  fMatTarget = new SLArMaterial();

  fMatSteel->SetMaterialID("Steel");
  fMatSteel->BuildMaterialFromDB(db_file);

  fMatPlywood->SetMaterialID("Plywood"); 
  fMatPlywood->BuildMaterialFromDB(db_file); 

  fMatPolyurethane->SetMaterialID("Polyurethane"); 
  fMatPolyurethane->BuildMaterialFromDB(db_file); 

  fMatBPolyethilene->SetMaterialID("BoratedPolyethilene"); 
  fMatBPolyethilene->BuildMaterialFromDB(db_file); 

  fMatTarget->SetMaterialID("LAr");
  fMatTarget->BuildMaterialFromDB(db_file);

  fMatWorld ->SetMaterialID("Air");
  fMatWorld ->BuildMaterialFromDB(db_file);
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

void SLArDetTPC::BuildCryostatStructure(const rapidjson::Value& jcryo) {
  assert(jcryo.IsArray()); 
  printf("SLArDetTPC::BuildCryostatStructure\n");

  if (!fCryostat) fCryostat = new SLArBaseDetModule();

  G4double tgtZ = fGeoInfo->GetGeoPar("target_z");
  G4double tgtY = fGeoInfo->GetGeoPar("target_y");
  G4double tgtX = fGeoInfo->GetGeoPar("target_x");

  G4double cryostat_tk = 0.; 

  // compute total thickness
  for (const auto &layer : jcryo.GetArray()) {
    if (layer.HasMember("thickness")) 
      cryostat_tk += fGeoInfo->ParseJsonVal(layer["thickness"]); 
  }

  fCryostat->GetGeoInfo()->RegisterGeoPar("cryostat_tk", cryostat_tk); 

  G4double halfSize[3] = {tgtX*0.5 + cryostat_tk, 
                          tgtY*0.5 + cryostat_tk, 
                          tgtZ*0.5 + cryostat_tk}; 
  for (const auto &layer : jcryo.GetArray()) {
    if (layer.HasMember("thickness")) {
      G4double tk_ = SLArGeoInfo::ParseJsonVal(layer["thickness"]);
      if (tk_ == 0.) continue;

      assert(layer.HasMember("id")); 
      assert(layer.HasMember("name")); 
      assert(layer.HasMember("material")); 
      for (size_t k=0; k<3; k++) halfSize[k] -= tk_; 
      
      SLArCryostatLayer* ll = new SLArCryostatLayer(
          layer["name"].GetString(), halfSize, 
          SLArGeoInfo::ParseJsonVal(layer["thickness"]), 
          layer["material"].GetString());

      fCryostatStructure.insert(std::make_pair(layer["id"].GetInt(), ll)); 
    }
  }

  printf("SLArDetTPC::BuildCryostatStructure: Cryostat layered structure built\n");
  for (const auto& l : fCryostatStructure) {
    printf("%i: %s %g mm %s\n", 
        l.first, l.second->fName.c_str(), l.second->fThickness, 
        l.second->fMaterialName.c_str());
  }
  return; 
}

void SLArDetTPC::BuildCryostat()
{
  G4cerr << "SLArDetTPC::BuildCryostat()" << G4endl;
  if (!fCryostat) fCryostat = new SLArBaseDetModule(); 
  G4double tgtZ         = fGeoInfo->GetGeoPar("target_z");
  G4double tgtY         = fGeoInfo->GetGeoPar("target_y");
  G4double tgtX         = fGeoInfo->GetGeoPar("target_x");
  G4double cryo_tk      = fCryostat->GetGeoPar("cryostat_tk"); 
   
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Create Tank 
  G4double x_ = tgtX*0.5 + cryo_tk;
  G4double y_ = tgtY*0.5 + cryo_tk;
  G4double z_ = tgtZ*0.5 + cryo_tk;
  // Create outer box 
  fBoxOut = new G4Box("fBoxOut_solid", 
      x_, y_, z_); 

  // Create inner box 
  fBoxInn = new G4Box("fBoxInn_solid", 
      x_-cryo_tk, y_-cryo_tk, z_-cryo_tk);

  G4SubtractionSolid* cryostat_solid = 
    new G4SubtractionSolid("cryostat_solid", 
        fBoxOut, fBoxInn, 0, G4ThreeVector(0,0,0));

  // Create Cryostat container volume
  G4cerr << "Create Cryostat" << G4endl;
  fCryostat->SetGeoPar("cryostat_x", 2*x_);
  fCryostat->SetGeoPar("cryostat_y", 2*y_);
  fCryostat->SetGeoPar("cryostat_z", 2*z_);
  fCryostat->SetMaterial(fMatWorld->GetMaterial());
  fCryostat->SetSolidVolume(cryostat_solid);

  fCryostat->SetLogicVolume(
    new G4LogicalVolume(fCryostat->GetModSV(), 
      fCryostat->GetMaterial(),
      "CryostatLV", 0, 0, 0)
    );

  // -------------------------------------------------------------------------
  // create cryostat layers

  for (const auto& ll : fCryostatStructure) {
    auto layer = ll.second;
    layer->fMaterial = SLArMaterial::FindInMaterialTable(layer->fMaterialName); 
    layer->fModule = BuildCryostatLayer(layer->fName, 
        layer->fHalfSizeX, layer->fHalfSizeY, layer->fHalfSizeZ, 
        layer->fThickness, layer->fMaterial); 
    layer->fModule->GetModPV(
        layer->fName, 0, G4ThreeVector(0, 0, 0), fCryostat->GetModLV(), 
        false, ll.first);
  }

  return; 
}

SLArBaseDetModule* SLArDetTPC::BuildCryostatLayer(
    G4String name, 
    G4double x_, G4double y_, G4double z_, G4double tk_, 
    G4Material* mat) {

  G4Box* b_out = new G4Box("b_out_"+name, x_+tk_, y_+tk_, z_+tk_); 
  G4Box* b_in  = new G4Box("b_in_" +name, x_    , y_    , z_    ); 

  G4SubtractionSolid* solid = new G4SubtractionSolid(name+"_solid", 
      b_out, b_in, 0, G4ThreeVector(0, 0, 0)); 

  SLArBaseDetModule* mod = new SLArBaseDetModule(); 
  mod->SetMaterial(mat); 
  mod->SetSolidVolume(solid); 
  mod->SetLogicVolume(new G4LogicalVolume(
        mod->GetModSV(), mod->GetMaterial(), name+"LV", 0, 0, 0)); 

  // create a daughter volume on the -z face to be used as a sensitive detector 
  // for neutron shielding studies

  G4Box* b_test_sv = new G4Box("b_test_sv_"+name, x_, y_, 0.5*tk_); 
  G4LogicalVolume* b_test_lv = new G4LogicalVolume(b_test_sv, mat, "b_test_lv_"+name, 
      0, 0, 0, 0); 
  new G4PVPlacement(0, G4ThreeVector(0, 0, -z_-0.5*tk_), b_test_lv, "b_test_pv_"+name, 
      mod->GetModLV(), false, 8, true); 

  return mod; 
}

void SLArDetTPC::BuildTarget()
{
  G4cerr << "SLArDetTPC::BuildTarget()" << G4endl;
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

void SLArDetTPC::BuildTPC() 
{

  G4cerr << "SLArDetTPC::BuildTank()" << G4endl;
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Building the Target                                 //
  G4cerr << "\tBuilding Cryostat and LAr volume" << G4endl;
  BuildCryostat();
  BuildTarget();
  
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Building a "empty" LV as Tank container             //
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  fModLV
    = new G4LogicalVolume(fBoxOut,
        fMatWorld->GetMaterial(),
        "TankLV",0,0,0);
 
  //* * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Place Cryostat
  G4cerr << "\tPlacing Cryostat" << G4endl;

  fCryostat->GetModPV("Vessel", 0, G4ThreeVector(0, 0, 0), 
      fModLV, false, 24);


  //* * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Place LAr
  G4cerr << "\tPlacing LAr Target" << G4endl;

  fTarget->GetModPV("Target", 0, 
      G4ThreeVector(0, 0, 0), 
      fModLV, false, 24);

  G4cerr << "end of method\n" << G4endl;
}


void SLArDetTPC::SetVisAttributes()
{
  G4cout << "SLArDetTPC::SetVisAttributes()" << G4endl;

  G4VisAttributes* visAttributes = new G4VisAttributes();

  visAttributes = new G4VisAttributes();
  visAttributes->SetColour(0.611, 0.847, 0.988, 0.6);
  visAttributes->SetVisibility(true); 
  if (fCryostat) {
    fCryostat->GetModLV()->SetVisAttributes( visAttributes );
    for (size_t j=0; j<fCryostat->GetModLV()->GetNoDaughters(); j++) {
      auto pv = fCryostat->GetModLV()->GetDaughter(j); 
      printf("%s\n", pv->GetName().c_str());
      pv->GetLogicalVolume()->SetVisAttributes(visAttributes); 
       
    }
  }
  visAttributes = new G4VisAttributes();
  visAttributes->SetColor(0.607, 0.847, 0.992, 0.4);
  if (fTarget)
    fTarget->GetModLV()->SetVisAttributes(visAttributes);
  fModLV->SetVisAttributes( G4VisAttributes(false) );

  return;
}
