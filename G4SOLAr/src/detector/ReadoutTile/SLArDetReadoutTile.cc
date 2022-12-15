/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetReadoutTile.cc
 * @created     : giovedì lug 14, 2022 09:33:56 CEST
 */


#include "detector/ReadoutTile/SLArDetReadoutTile.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PVParameterised.hh"
#include "G4VPhysicalVolume.hh"

#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4VisAttributes.hh"
#include "G4MaterialPropertyVector.hh"

SLArDetReadoutTile::SLArDetReadoutTile() : SLArBaseDetModule(),
  fPerfectQE(false),
  fBasePCB(nullptr), fChargePix(nullptr), 
  fSiPM(nullptr), fSiPMActive(nullptr), fUnitCell(nullptr),
  fMatReadoutTile(nullptr), fMatPCB(nullptr), 
  fMatSiPM(nullptr), fMatSiPMCapsule(nullptr), 
  fSkinSurface(nullptr)
{  
  fGeoInfo = new SLArGeoInfo();
}

SLArDetReadoutTile::SLArDetReadoutTile(const SLArDetReadoutTile &detReadoutTile) 
  : SLArBaseDetModule(detReadoutTile)
{
  fPerfectQE   = detReadoutTile.fPerfectQE;
  fGeoInfo     = detReadoutTile.fGeoInfo;

  fMatReadoutTile = new SLArMaterial(*detReadoutTile.fMatReadoutTile); 
  fMatPCB = new SLArMaterial(*detReadoutTile.fMatPCB);
  fMatSiPM   = new SLArMaterial(*detReadoutTile.fMatSiPM);
  fMatSiPMCapsule = new SLArMaterial(*detReadoutTile.fMatSiPMCapsule); 

  fBasePCB = new SLArBaseDetModule(*detReadoutTile.fBasePCB); 
  fSiPM = new SLArBaseDetModule(*detReadoutTile.fSiPM); 
  fSiPMActive = new SLArBaseDetModule(*detReadoutTile.fSiPMActive); 
  fUnitCell = new SLArBaseDetModule(*detReadoutTile.fUnitCell); 
  fChargePix = new SLArBaseDetModule(*detReadoutTile.fChargePix); 
}

SLArDetReadoutTile::~SLArDetReadoutTile() {
  G4cerr << "Deleting SLArDetReadoutTile... " <<  G4endl;

  if (fBasePCB)   {delete fBasePCB; fBasePCB = 0;}
  if (fChargePix) {delete fChargePix; fChargePix = 0;}
  if (fSiPM)      {delete fSiPM; fSiPM = 0;}
  if (fSiPMActive){delete fSiPMActive; fSiPMActive = 0;} 
  if (fMatReadoutTile) {delete fMatReadoutTile; fMatReadoutTile = 0;}
  if (fMatPCB){delete fMatPCB; fMatPCB = 0;}
  if (fMatSiPM)   {delete fMatSiPM; fMatSiPM = 0;}
  if (fMatSiPMCapsule)   {delete fMatSiPMCapsule; fMatSiPMCapsule = 0;}
  G4cerr << "SLArDetReadoutTile DONE" <<  G4endl;
}

void SLArDetReadoutTile::BuildPCB()
{
  G4cerr << "Building ReadoutTile PCB base" << G4endl;

  fBasePCB = new SLArBaseDetModule();
  fBasePCB->SetGeoPar(fGeoInfo->GetGeoPair("tile_z"));
  fBasePCB->SetGeoPar(fGeoInfo->GetGeoPair("tile_x"));
  fBasePCB->SetGeoPar(fGeoInfo->GetGeoPair("tile_y"));

  fBasePCB->SetMaterial(fMatPCB->GetMaterial());

  G4VSolid* lgbox = 
    new G4Box("PCBBaseSolid", 
        0.5*fBasePCB->GetGeoPar("tile_x"),
        0.5*fBasePCB->GetGeoPar("tile_y"),
        0.5*fBasePCB->GetGeoPar("tile_z"));

  fBasePCB->SetSolidVolume(lgbox);
  fBasePCB->SetLogicVolume(
    new G4LogicalVolume(fBasePCB->GetModSV(), 
      fBasePCB->GetMaterial(),
      "PCBBaseLV", 0, 0, 0)
    );

  printf("tile dimensions: %.2f %.2f, %.2f mm\n", 
      fGeoInfo->GetGeoPar("tile_x"), fGeoInfo->GetGeoPar("tile_y"), fGeoInfo->GetGeoPar("tile_z"));
}

void SLArDetReadoutTile::BuildSiPM()
{
  G4cout << "Building ReadoutTile SiPMs..." << G4endl;
  double fill_factor = fSiPM->GetGeoPar("sipm_fill_factor"); 
  double x_ = fSiPM->GetGeoPar("sipm_x"); 
  double y_ = fSiPM->GetGeoPar("sipm_y"); 
  double z_ = fSiPM->GetGeoPar("sipm_z"); 
  double d_ = 0.25*((x_ + z_) - sqrt(pow(x_ + z_, 2) - 4*x_*z_*(1-fill_factor))); 
  double x  = x_ - 2*d_; 
  double z  = z_ - 2*d_; 

  fSiPMActive = new SLArBaseDetModule(); 

  fSiPMActive = new SLArBaseDetModule(); 
  fSiPMActive->SetGeoPar("active_sipm_y", y_); 
  fSiPMActive->SetGeoPar("active_sipm_x", x ); 
  fSiPMActive->SetGeoPar("active_sipm_z", z ); 
  
  fSiPM->SetMaterial(fMatReadoutTile->GetMaterial());
  fSiPMActive->SetMaterial(fMatSiPM->GetMaterial()); 

  fSiPM->SetSolidVolume(
        new G4Box("SiPMBox", 
          0.5*x_, 0.5*y_, 0.5*z_));
 
  fSiPM->SetLogicVolume(
      new G4LogicalVolume(fSiPM->GetModSV(), 
        fMatReadoutTile->GetMaterial(), "SiPM_lv", 0, 0, 0)
      );

  fSiPMActive->SetSolidVolume(
      new G4Box("SiPMActiveBox", 0.5*x, 0.5*y_, 0.5*z) );

  fSiPMActive->SetLogicVolume(
    new G4LogicalVolume(fSiPMActive->GetModSV(), 
        fMatSiPM->GetMaterial(), "SiPMActive_lv", 0, 0, 0)
      );

  G4SubtractionSolid* sipm_passive_box = new G4SubtractionSolid("SiPMCapsuleBox", 
      fSiPM->GetModSV(), fSiPMActive->GetModSV(), 0, G4ThreeVector(0, 0, 0)); 

  G4LogicalVolume* capsule_lv = 
    new G4LogicalVolume(sipm_passive_box, 
        fMatSiPMCapsule->GetMaterial(), "SiPMCapsule_lv", 0, 0, 0); 

  new G4PVPlacement(0, G4ThreeVector(0, 0, 0), capsule_lv, "SiPMCapsulePV",
      fSiPM->GetModLV(), 0, 251, true);

  fSiPMActive->GetModPV("SiPMActivePV", 0, G4ThreeVector(0, 0, 0), 
      fSiPM->GetModLV(), 0, 250); 
}

void SLArDetReadoutTile::BuildChargePix()
{
  G4cout << "Building ReadoutTile ChargePixel..." << G4endl;
  fChargePix->SetMaterial(fMatSiPM->GetMaterial());

  fChargePix->SetSolidVolume(
        new G4Box("pixel_box", 
          0.5*fChargePix->GetGeoPar("pix_x"),
          0.5*fChargePix->GetGeoPar("pix_y"),
          0.5*fChargePix->GetGeoPar("pix_z"))
        );
 
  fChargePix->SetLogicVolume(
      new G4LogicalVolume(fChargePix->GetModSV(), 
        fChargePix->GetMaterial(), "pixel_lv", 0, 0, 0)
      );
}

void SLArDetReadoutTile::BuildUnitCell() {
  //--------------------------  Build Unit Cell components
  BuildSiPM();
  BuildChargePix(); 

  G4double hl = 0, hq = 0;

  if (fSiPM) hl = fSiPM->GetGeoPar("sipm_y"); 
  if (fChargePix) hq = fChargePix->GetGeoPar("pix_y"); 
  fUnitCell->SetGeoPar("cell_y", std::max(hl, hq)); 
  fUnitCell->SetSolidVolume(
   new G4Box("tileCellBox", 
      0.5*fUnitCell->GetGeoPar("cell_x"), 
      0.5*fUnitCell->GetGeoPar("cell_y"), 
      0.5*fUnitCell->GetGeoPar("cell_z"))); 
  fUnitCell->SetLogicVolume( 
      new G4LogicalVolume(
        fUnitCell->GetModSV(), 
        fMatReadoutTile->GetMaterial(), 
        "rdtile_cell_lv")); 
  fUnitCell->GetModLV()->SetVisAttributes( G4VisAttributes(false) ); 

  
  for (const auto& comp : fCellStructure) {
    G4ThreeVector yshift(0, 0, 0); 
    if (comp.fMod == fChargePix && hq < hl) yshift.setY(0.5*(hq-hl));
    else if (comp.fMod == fSiPM && hl < hq) yshift.setY(0.5*(hl-hq));
    comp.fMod->GetModPV(comp.fName, 0, comp.fPos+yshift, 
        fUnitCell->GetModLV(),true, comp.fCopyNo); 
  }

}

void SLArDetReadoutTile::BuildReadoutTile()
{
  //--------------------------  Build ReadoutTile components
  BuildPCB();

  BuildUnitCell(); 

  //--------- Building a "empty" LV as ReadoutTile container
  G4cout << "SLArDetReadoutTile::BuildReadoutTile()" << G4endl;
  fhTot = fGeoInfo->GetGeoPar("tile_y") + fUnitCell->GetGeoPar("cell_y"); 

  G4VSolid* ReadoutTile_box = new G4Box("ReadoutTile",
      fGeoInfo->GetGeoPar("tile_x")*0.5,
      fhTot*0.5,
      fGeoInfo->GetGeoPar("tile_z")*0.5
      );

  fModLV
    = new G4LogicalVolume(ReadoutTile_box, 
        fMatReadoutTile->GetMaterial(),
        "ReadoutTileLV",0,0,0);

  //----------------------------Place ReadoutTile components
  // 1. The PCB base
  G4double h = 0*CLHEP::mm;
  G4cout<<"GetModPV PCB base..." << G4endl; 
  fBasePCB->GetModPV("ReadoutTilePCB", 0, 
      G4ThreeVector(0, -0.5*(fhTot-fGeoInfo->GetGeoPar("tile_y")), 0),
      fModLV, false, 200);

  // 3. Create a volume parametrization instance 
  G4double cell_z = fUnitCell->GetGeoPar("cell_z"); 
  G4double cell_y = fUnitCell->GetGeoPar("cell_y"); 
  G4double cell_x = fUnitCell->GetGeoPar("cell_x"); 
  G4double tile_z = fGeoInfo->GetGeoPar("tile_z"); 
  G4double tile_x = fGeoInfo->GetGeoPar("tile_x"); 

  G4int n_z = floor(tile_z / cell_z); 
  G4int n_x = floor(tile_x / cell_x); 

  SLArRTileParametrization* rowParametrization = 
    new SLArRTileParametrization(kZAxis, 
        G4ThreeVector(0, 0, -0.5*cell_z*(n_z-1)), 
        cell_z); 
  printf("Creating a row of %i sensor cells...\n", n_z);

  G4Box* cell_row_box = new G4Box("tileCellRow",0.5*cell_x,0.5*cell_y,0.5*cell_z*n_z); 
  G4LogicalVolume* cell_row_lv = new G4LogicalVolume(cell_row_box, fMatReadoutTile->GetMaterial(), "rdtile_cell_row_lv"); 
  cell_row_lv->SetVisAttributes( G4VisAttributes(false) ); 
  new G4PVParameterised("cell_row", fUnitCell->GetModLV(), cell_row_lv, kZAxis, n_z,
      rowParametrization, true); 
  
  // 4. Full sensor plane
  printf("Creating %i repilacas of rows...\n", n_x);
  SLArRTileParametrization* tplaneParametrization = 
    new SLArRTileParametrization(kXAxis, 
        G4ThreeVector(-0.5*cell_x*(n_x-1), 0, 0), 
        cell_x); 

  G4Box* cell_plane_box = new G4Box("tileCellPlane", 
      0.5*cell_x*n_x, 0.5*cell_y, 0.5*cell_z*n_z); 
  G4LogicalVolume* cell_plane_lv = new G4LogicalVolume(cell_plane_box, 
      fMatReadoutTile->GetMaterial(), "rdtile_cell_plane_lv"); 
  cell_plane_lv->SetVisAttributes( G4VisAttributes(false) ); 
  //new G4PVReplica("cell_plane", cell_row_lv, cell_plane_lv, kXAxis, 10, 3*dx); 
  new G4PVParameterised("cell_plane", cell_row_lv, cell_plane_lv, kXAxis, n_x, tplaneParametrization, true); 

  // 5. Final assembly (PCB + sensor plane)
  G4cout<<"Final placement..." << G4endl; 
  new G4PVPlacement(
      0, G4ThreeVector(0., 0.5*(fhTot-cell_y), 0.), 
      cell_plane_lv, "ReadoutTileSensors",fModLV, false, 50, false);

   return;
}


void SLArDetReadoutTile::SetVisAttributes()
{
  G4VisAttributes* visAttributes = new G4VisAttributes();
  visAttributes->SetColor(0.0824, 0.635, 0.019);
  fBasePCB->GetModLV()->SetVisAttributes( visAttributes );

  visAttributes = new G4VisAttributes( G4Color(0.753, 0.753, 0.753) );
  fSiPM->GetModLV()->SetVisAttributes( visAttributes );

  if (fChargePix) {
    visAttributes = new G4VisAttributes( G4Color(0.921, 0.659, 0.007) );
    fChargePix->GetModLV()->SetVisAttributes( visAttributes );
  }

  visAttributes = new G4VisAttributes();
  visAttributes->SetColor(0.305, 0.294, 0.345, 0.0);
  fModLV->SetVisAttributes( visAttributes );

  return;
}

SLArBaseDetModule* SLArDetReadoutTile::GetSiPMActive()
{
  return fSiPMActive;
}


SLArMaterial* SLArDetReadoutTile::GetSiPMActiveMaterial()
{
  return fMatSiPM;
}


void SLArDetReadoutTile::SetPerfectQE(G4bool kQE)
{
  if (fPerfectQE==kQE) return;
  else 
  {     
    fPerfectQE = kQE;
    G4cout << "SLArDetReadoutTile::SetPerfectQE: Setting 100% QE between "
           << "1 and 5 eV" << G4endl;
    G4double phEne[2] = {1*CLHEP::eV, 13*CLHEP::eV};
    G4double eff  [2] = {1.0 , 1.0 };
    
    fMatSiPM->GetMaterialOpticalSurf()
               ->GetMaterialPropertiesTable()
                  ->AddProperty("EFFICIENCY", phEne, eff, 2);  
  }

  return;
}

void SLArDetReadoutTile::BuildMaterial(G4String materials_db)
{
  fMatPCB         = new SLArMaterial();
  fMatSiPM        = new SLArMaterial();
  fMatChargePix   = new SLArMaterial(); 
  fMatReadoutTile = new SLArMaterial();
  fMatSiPMCapsule = new SLArMaterial(); 

  fMatReadoutTile->SetMaterialID("LAr");
  fMatReadoutTile->BuildMaterialFromDB(materials_db);

  fMatPCB->SetMaterialID("PCB");
  fMatPCB->BuildMaterialFromDB(materials_db);

  fMatChargePix->SetMaterialID("Steel");
  fMatChargePix->BuildMaterialFromDB(materials_db);

  fMatSiPM->SetMaterialID("SiliconActive");
  fMatSiPM->BuildMaterialFromDB(materials_db);

  fMatSiPMCapsule->SetMaterialID("SiliconPassive");
  fMatSiPMCapsule->BuildMaterialFromDB(materials_db);
}

void SLArDetReadoutTile::BuildComponentsDefinition(const rapidjson::Value& comps) 
{
  assert(comps.IsArray()); 
  for (const auto& comp : comps.GetArray()) {
    assert(comp.HasMember("name")); 
    assert(comp.HasMember("dimensions")); 
    SLArBaseDetModule* mod = nullptr;
    if (std::strcmp(comp["name"].GetString(), "pixel") == 0) {
      if (fChargePix) mod = fChargePix; 
      else {fChargePix = new SLArBaseDetModule(); mod = fChargePix;}
      printf("SLArDetReadoutTile::BuildComponentsDefinition: %s [%p]\n", 
          comp["name"].GetString(), static_cast<void*>(mod));
    } 
    else if (std::strcmp(comp["name"].GetString(), "sipm") == 0) {
      if (fSiPM) mod = fSiPM; 
      else {fSiPM = new SLArBaseDetModule(); mod = fSiPM;}
      printf("SLArDetReadoutTile::BuildComponentsDefinition: %s [%p]\n", 
          comp["name"].GetString(), static_cast<void*>(mod));
    }
    else {
      printf("SLArDetReadoutTile::BuildComponentsDefinition: I don't know what a %s is.\n", comp["name"].GetString());
    }

    assert(comp["dimensions"].IsArray()); 
    mod->GetGeoInfo()->ReadFromJSON(comp["dimensions"]); 
  }
}

void SLArDetReadoutTile::BuildUnitCellStructure(const rapidjson::Value& celldef) {
  assert(celldef.HasMember("dimensions")); 
  assert(celldef.HasMember("lineup")); 
  if (!fUnitCell) fUnitCell = new SLArBaseDetModule(); 
  fUnitCell->GetGeoInfo()->ReadFromJSON(celldef["dimensions"]);

  assert(celldef["lineup"].IsArray()); 
  for (const auto& comp : celldef["lineup"].GetArray()) {
    SLArBaseDetModule* mod = nullptr; 
    if (std::strcmp(comp["component"].GetString(), "pixel") == 0) mod = fChargePix; 
    else if (std::strcmp(comp["component"].GetString(), "sipm") == 0) mod = fSiPM; 
    else {
      printf("SLArDetReadoutTile::BuildUnitCellStructure: Unknown component %s in lineup\n", comp["component"].GetString());
    }

    G4String name_ = comp["name"].GetString(); 
    G4int copy_ = comp["copy"].GetInt(); 
    G4ThreeVector pos_ = G4ThreeVector(0, 0, 0); 
    if (comp.HasMember("pos_x")) pos_.setX(SLArGeoInfo::ParseJsonVal(comp["pos_x"]));
    if (comp.HasMember("pos_y")) pos_.setY(SLArGeoInfo::ParseJsonVal(comp["pos_y"]));
    if (comp.HasMember("pos_z")) pos_.setZ(SLArGeoInfo::ParseJsonVal(comp["pos_z"]));

    fCellStructure.push_back( SUnitCellComponent(name_, copy_, mod, pos_) );
  }

  assert(celldef.HasMember("pixelmap")); 
  BuildUnitCellPixMap(celldef["pixelmap"]); 
}


void SLArDetReadoutTile::BuildUnitCellPixMap(const rapidjson::Value& pixblueprint) {
  assert(pixblueprint.IsArray()); 
  for (const auto& pix : pixblueprint.GetArray()) {
    assert(pix.HasMember("name")); 
    assert(pix.HasMember("edges")); 
    assert(pix["edges"].IsArray());
    SUnitCellPixelArea pixArea(pix["name"].GetString()); 

    for (const auto &edge : pix["edges"].GetArray()) {
      G4ThreeVector pos_ = G4ThreeVector(0, 0, 0); 
      if (edge.HasMember("x")) pos_.setX(SLArGeoInfo::ParseJsonVal(edge["x"]));
      if (edge.HasMember("y")) pos_.setY(SLArGeoInfo::ParseJsonVal(edge["y"]));
      if (edge.HasMember("z")) pos_.setZ(SLArGeoInfo::ParseJsonVal(edge["z"]));

      pixArea.fEdges.push_back(pos_); 
    }

    fCellPixelMap.push_back(pixArea); 
  }
}



G4LogicalSkinSurface* SLArDetReadoutTile::BuildLogicalSkinSurface() {
  fSkinSurface = 
    new G4LogicalSkinSurface(
        "SiPM_LgSkin", 
        fSiPMActive->GetModLV(), 
        fMatSiPM->GetMaterialOpticalSurf());

  return fSkinSurface;
}

SLArDetReadoutTile::SLArRTileParametrization::SLArRTileParametrization(
    EAxis replica_axis, G4ThreeVector start_pos, G4double spacing) 
  : fReplicaAxis(replica_axis), fStartPos(start_pos), fSpacing(spacing) 
{

  if      (fReplicaAxis == kXAxis) {fAxisVector = G4ThreeVector(1, 0, 0);} 
  else if (fReplicaAxis == kYAxis) {fAxisVector = G4ThreeVector(0, 1, 0);} 
  else                             {fAxisVector = G4ThreeVector(0, 0, 1);} 

  return; 
}

void SLArDetReadoutTile::SLArRTileParametrization::ComputeTransformation(
    G4int copyNo, G4VPhysicalVolume* physVol) const {
  G4ThreeVector origin = fStartPos; 
  origin += fAxisVector*(copyNo)*fSpacing; 

  physVol->SetTranslation(origin); 
  physVol->SetRotation(0); 
  return; 
}

