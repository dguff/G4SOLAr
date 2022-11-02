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
  fTarget   (nullptr), fCryostat (nullptr)  ,  
  fBoxOut   (nullptr), fBoxInn (nullptr)  ,
  fMatWorld (nullptr), fMatSteel(nullptr) , 
  fMatPlywood(nullptr), fMatPolyurethane(nullptr), 
  fMatBPolyethilene(nullptr), fMatTarget(nullptr)
{

  fGeoInfo = new SLArGeoInfo();
}


SLArDetTank::~SLArDetTank() {
  std::cerr << "Deleting SLArDetTank..." << std::endl;
  if (fTarget   ) {delete fTarget   ; fTarget    = NULL;}
  if (fCryostat   ) {delete fCryostat   ; fCryostat    = NULL;}
  if (fWindow   ) {delete fWindow   ; fWindow    = NULL;}
  if (fBoxOut   ) {delete fBoxOut   ; fBoxOut    = NULL;}
  if (fBoxInn   ) {delete fBoxInn   ; fBoxInn    = NULL;}
  std::cerr << "SLArDetTank DONE" << std::endl;
}

void SLArDetTank::BuildMaterial(G4String db_file) 
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

void SLArDetTank::BuildDefalutGeoParMap() 
{
  G4cerr << "SLArDetTank::BuildGeoParMap()" << G4endl;
  fGeoInfo->RegisterGeoPar("target_y"       , 150.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("target_z"       , 200.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("target_x"       ,  60.0*CLHEP::cm); 
  fGeoInfo->RegisterGeoPar("cryo_tk"        ,   1.0*CLHEP::cm);
  G4cerr << "Exit method\n" << G4endl;
}

void SLArDetTank::BuildCryostat()
{
  G4cerr << "SLArDetTank::BuildVessel()" << G4endl;
  G4double tgtZ         = fGeoInfo->GetGeoPar("target_z");
  G4double tgtY         = fGeoInfo->GetGeoPar("target_y");
  G4double tgtX         = fGeoInfo->GetGeoPar("target_x");
  G4double out_tk       = fGeoInfo->GetGeoPar("outer_tk"); 
  G4double in_tk        = fGeoInfo->GetGeoPar("inner_tk"); 
  G4double foam_tk      = fGeoInfo->GetGeoPar("foam_tk"); 
  G4double wood_tk      = fGeoInfo->GetGeoPar("plywood_tk"); 
  G4double trpl_tk      = fGeoInfo->GetGeoPar("triplex_tk"); 
  G4double bplt_tk      = 0.0; 
  G4double tnkThck      = out_tk + 4*wood_tk + 2*foam_tk + trpl_tk + in_tk; 
  if (fGeoInfo->Contains("bplt_tk")) {
    bplt_tk = fGeoInfo->GetGeoPar("bplt_tk"); 
    tnkThck+= (2*wood_tk + 2*bplt_tk); 
  }

  
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

  G4SubtractionSolid* cryostat_solid = 
    new G4SubtractionSolid("cryostat_solid", 
        fBoxOut, fBoxInn, 0, G4ThreeVector(0,0,0));

  // Create Cryostat container volume
  G4cerr << "Create Cryostat" << G4endl;
  fCryostat = new SLArBaseDetModule();
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
  G4double halfSize[3] = {x_-out_tk, y_-out_tk, z_-out_tk}; 
  int iwood = 0; int ifoam = 0; int ibplt = 0;
  std::vector<SLArCryostatLayer> layers; 
  layers.push_back(SLArCryostatLayer("outer", halfSize, out_tk, fMatSteel) ); 
  for (int k=0; k<3; k++) halfSize[k] -= wood_tk; 
  layers.push_back(
      SLArCryostatLayer("wood_"+std::to_string(iwood), 
        halfSize, wood_tk, fMatPlywood));
  iwood++;
  if (bplt_tk > 0) {
    for (int k=0; k<3; k++) halfSize[k] -= bplt_tk; 
    layers.push_back(
        SLArCryostatLayer("BoratedPolyethilene_"+std::to_string(ibplt), 
          halfSize, bplt_tk, fMatBPolyethilene)); 
    ibplt++; 
    for (int k=0; k<3; k++) halfSize[k] -= wood_tk; 
    layers.push_back(
        SLArCryostatLayer("wood_"+std::to_string(iwood), 
          halfSize, wood_tk, fMatPlywood));
    iwood++;
  }
  for (int k=0; k<3; k++) halfSize[k] -= foam_tk; 
  layers.push_back(
      SLArCryostatLayer("Polyurethane_"+std::to_string(ifoam), 
        halfSize, foam_tk, fMatPolyurethane)); 
  ifoam++; 
  for (int k=0; k<3; k++) halfSize[k] -= wood_tk; 
  layers.push_back(
      SLArCryostatLayer("wood_"+std::to_string(iwood), 
        halfSize, wood_tk, fMatPlywood));
  iwood++;
  for (int k=0; k<3; k++) halfSize[k] -= trpl_tk; 
  layers.push_back(
      SLArCryostatLayer("triplex", halfSize, trpl_tk, fMatSteel));
  for (int k=0; k<3; k++) halfSize[k] -= wood_tk; 
  layers.push_back(
      SLArCryostatLayer("wood_"+std::to_string(iwood), 
        halfSize, wood_tk, fMatPlywood));
  iwood++;
  for (int k=0; k<3; k++) halfSize[k] -= foam_tk; 
  layers.push_back(
      SLArCryostatLayer("Polyurethane_"+std::to_string(ifoam), 
        halfSize, foam_tk, fMatPolyurethane)); 
  ifoam++; 
  for (int k=0; k<3; k++) halfSize[k] -= wood_tk; 
  layers.push_back(
      SLArCryostatLayer("wood_"+std::to_string(iwood), 
        halfSize, wood_tk, fMatPlywood));
  iwood++;
  if (bplt_tk > 0) {
    for (int k=0; k<3; k++) halfSize[k] -= bplt_tk; 
    layers.push_back(
        SLArCryostatLayer("BoratedPolyethilene_"+std::to_string(ibplt), 
          halfSize, bplt_tk, fMatBPolyethilene)); 
    ibplt++; 
    for (int k=0; k<3; k++) halfSize[k] -= wood_tk; 
    layers.push_back(
        SLArCryostatLayer("wood_"+std::to_string(iwood), 
          halfSize, wood_tk, fMatPlywood));
    iwood++;
  }

  int imod = 1; 
  for (const auto& layer : layers) {
    auto mod = BuildCryostatLayer(layer.fName, 
        layer.fHalfSizeX, layer.fHalfSizeY, layer.fHalfSizeZ, 
        layer.fThickness, layer.fMaterial); 
    mod->GetModPV(layer.fName, 0, G4ThreeVector(0, 0, 0), fCryostat->GetModLV(), 
        false, imod);
    imod++; 
  }

}

SLArBaseDetModule* SLArDetTank::BuildCryostatLayer(G4String name, G4double x_, G4double y_, G4double z_, G4double tk_, SLArMaterial* mat) {

  G4Box* b_out = new G4Box("b_out_"+name, x_+tk_, y_+tk_, z_+tk_); 
  G4Box* b_in  = new G4Box("b_in_" +name, x_    , y_    , z_    ); 

  G4SubtractionSolid* solid = new G4SubtractionSolid(name+"_solid", 
      b_out, b_in, 0, G4ThreeVector(0, 0, 0)); 

  SLArBaseDetModule* mod = new SLArBaseDetModule(); 
  mod->SetMaterial(mat->GetMaterial()); 
  mod->SetSolidVolume(solid); 
  mod->SetLogicVolume(new G4LogicalVolume(
        mod->GetModSV(), mod->GetMaterial(), name+"LV", 0, 0, 0)); 

  return mod; 
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

void SLArDetTank::BuildTPC() 
{

  G4cerr << "SLArDetTank::BuildTank()" << G4endl;
  //* * * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Building the Target                                 //
  G4cerr << "\tBuilding Vessel, Window and Target" << G4endl;
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
  // Place Steel Tank
  G4cerr << "\tPlacing Vessel" << G4endl;

  fCryostat->GetModPV("Vessel", 0, G4ThreeVector(0, 0, 0), 
      fModLV, false, 24);


  //* * * * * * * * * * * * * * * * * * * * * * * * * *//
  // Place LAr
  G4cerr << "\tPlacing Target" << G4endl;

  fTarget->GetModPV("Target", 0, 
      G4ThreeVector(0, 0, 0), 
      fModLV, false, 24);

  G4cerr << "end of method\n" << G4endl;
}


void SLArDetTank::SetVisAttributes()
{
  G4cout << "SLArDetTank::SetVisAttributes()" << G4endl;

  G4VisAttributes* visAttributes = new G4VisAttributes();

  visAttributes = new G4VisAttributes();
  visAttributes->SetColour(0.611, 0.847, 0.988, 0.6);
  visAttributes->SetVisibility(false); 
  if (fCryostat) {
    fCryostat->GetModLV()->SetVisAttributes( visAttributes );
    for (size_t j=0; j<fCryostat->GetModLV()->GetNoDaughters(); j++) {
      printf("%s\n", fCryostat->GetModLV()->GetDaughter(j)->GetName().c_str());
    }
  }
  visAttributes = new G4VisAttributes();
  visAttributes->SetColor(0.607, 0.847, 0.992, 0.4);
  if (fTarget)
    fTarget->GetModLV()->SetVisAttributes(visAttributes);
  fModLV->SetVisAttributes( G4VisAttributes(false) );

  return;
}
