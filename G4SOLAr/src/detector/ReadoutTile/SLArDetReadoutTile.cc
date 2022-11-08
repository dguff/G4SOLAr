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
  fBasePCB(nullptr), fChargePix(nullptr), fSiPM(nullptr), fSiPMActive(nullptr),
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
}

void SLArDetReadoutTile::BuildSiPM()
{
  G4cout << "Building ReadoutTile SiPMs..." << G4endl;
  double fill_factor = fGeoInfo->GetGeoPar("sipm_fill_factor"); 
  double x_ = fGeoInfo->GetGeoPar("sipm_x"); 
  double y_ = fGeoInfo->GetGeoPar("sipm_y"); 
  double z_ = fGeoInfo->GetGeoPar("sipm_z"); 
  double d_ = 0.25*((x_ + z_) - sqrt(pow(x_ + z_, 2) - 4*x_*z_*(1-fill_factor))); 
  double x  = x_ - 2*d_; 
  double z  = z_ - 2*d_; 

  fSiPM = new SLArBaseDetModule();
  fSiPMActive = new SLArBaseDetModule(); 

  fSiPM->SetGeoPar(fGeoInfo->GetGeoPair("sipm_y"));
  fSiPM->SetGeoPar(fGeoInfo->GetGeoPair("sipm_x"));
  fSiPM->SetGeoPar(fGeoInfo->GetGeoPair("sipm_z"));

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
  fChargePix = new SLArBaseDetModule();
  fChargePix->SetGeoPar(fGeoInfo->GetGeoPair("pix_y"));
  fChargePix->SetGeoPar(fGeoInfo->GetGeoPair("pix_x"));
  fChargePix->SetGeoPar(fGeoInfo->GetGeoPair("pix_z"));

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

void SLArDetReadoutTile::BuildReadoutTile()
{
  //--------------------------  Build ReadoutTile components
  BuildPCB();
  BuildSiPM();
  BuildChargePix(); 


  //--------- Building a "empty" LV as ReadoutTile container
  G4cout << "SLArDetReadoutTile::BuildReadoutTile()" << G4endl;
  fhTot = fGeoInfo->GetGeoPar("tile_y") 
    + std::max(fGeoInfo->GetGeoPar("sipm_y"), fGeoInfo->GetGeoPar("pix_y"));

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

  // 2. The elementary-cell, consisting of 1 SiPM and 5 charge pixels
  // o X X
  // o X X
  // o o o
  h  = fGeoInfo->GetGeoPar("sipm_y"); 
  G4double hq = fGeoInfo->GetGeoPar("pix_y"); 
  G4double dx = 3.3*CLHEP::mm;
  G4cout<<"Placing sensors in a cell..." << G4endl; 
  G4Box* cell_box = new G4Box("tileCellBox", 
      0.5*fGeoInfo->GetGeoPar("sipm_x"), 
      0.5*h,
      0.5*fGeoInfo->GetGeoPar("sipm_z")); 
  G4LogicalVolume* cell_lv = new G4LogicalVolume(
      cell_box, fMatReadoutTile->GetMaterial(), "rdtile_cell_lv"); 
  cell_lv->SetVisAttributes( G4VisAttributes(false) ); 
  // place charge pixels inside cell
  //G4cout << "  - installing pixels" << G4endl; 
  fChargePix->GetModPV("qpix", 0, G4ThreeVector( -dx, 0.5*(hq-h), -dx), cell_lv); 
  fChargePix->GetModPV("qpix", 0, G4ThreeVector(  0., 0.5*(hq-h), -dx), cell_lv); 
  fChargePix->GetModPV("qpix", 0, G4ThreeVector( +dx, 0.5*(hq-h), -dx), cell_lv); 
  fChargePix->GetModPV("qpix", 0, G4ThreeVector( +dx, 0.5*(hq-h),  0.), cell_lv); 
  fChargePix->GetModPV("qpix", 0, G4ThreeVector( +dx, 0.5*(hq-h), +dx), cell_lv); 
  //G4cout << "  - installing sipm" << G4endl; 
  //-------------------------------------------------------- BENCHMARK GEOMETRY
  /*
   *  fSiPM->GetModPV("sipm", 0, G4ThreeVector(0.0, 0.0, 0.0), cell_lv, 2); 
   *
   *  // 3. A row of elementary cells
   *  G4cout<<"Creating a row of sensor cells..." << G4endl; 
   *  G4Box* cell_row_box = new G4Box("tileCellRow", 
   *      0.5*fGeoInfo->GetGeoPar("sipm_x"), 
   *      0.5*h, 
   *      0.5*fGeoInfo->GetGeoPar("tile_z"));
   *  G4LogicalVolume* cell_row_lv = 
   *    new G4LogicalVolume(cell_row_box, 
   *        fMatReadoutTile->GetMaterial(), 
   *        "rdtile_cell_row_lv"); 
   *  cell_row_lv->SetVisAttributes( G4VisAttributes(false) ); 
   *  new G4PVReplica("cell_row", cell_lv, cell_row_lv, kZAxis, 2, 
   *      0.5*fGeoInfo->GetGeoPar("tile_z"));  
   *  
   *  // 4. Full sensor plane
   *  G4cout<<"Creating replacas of rows..." << G4endl; 
   *  G4Box* cell_plane_box = new G4Box("tileCellPlane", 
   *      0.5*fGeoInfo->GetGeoPar("tile_x"), 
   *      0.5*h, 
   *      0.5*fGeoInfo->GetGeoPar("tile_z")); 
   *  G4LogicalVolume* cell_plane_lv = new G4LogicalVolume(cell_plane_box, 
   *      fMatReadoutTile->GetMaterial(), "rdtile_cell_plane_lv"); 
   *  cell_plane_lv->SetVisAttributes( G4VisAttributes(false) ); 
   *  new G4PVReplica("cell_plane", cell_row_lv, cell_plane_lv, kXAxis, 2, 
   *      0.5*fGeoInfo->GetGeoPar("tile_x")); 
   *
   *  // 5. Final assembly (PCB + sensor plane)
   *  G4cout<<"Final placement..." << G4endl; 
   *  new G4PVPlacement(
   *      0, G4ThreeVector(0., 0.5*(fhTot-h), 0.), 
   *      cell_plane_lv, "ReadoutTileSensors",fModLV, false, 50, false);
   *
   */
 
  //--------------------------------------------------------- Standard Geometry
  // place SiPM in unit cell
  fSiPM->GetModPV("sipm", 0, G4ThreeVector(-0.5*dx, 0, +0.5*dx), cell_lv, 2); 
  // 3. Create a volume parametrization instance 
  SLArRTileParametrization* rowParametrization = 
    new SLArRTileParametrization(kZAxis, G4ThreeVector(0, 0, -15*dx), 3*dx); 
  G4cout<<"Creating a row of sensor cells..." << G4endl; 
  G4Box* cell_row_box = new G4Box("tileCellRow", 1.5*dx, 0.5*h, 15*dx); 
  G4LogicalVolume* cell_row_lv = new G4LogicalVolume(cell_row_box, fMatReadoutTile->GetMaterial(), "rdtile_cell_row_lv"); 
  cell_row_lv->SetVisAttributes( G4VisAttributes(false) ); 
  new G4PVParameterised("cell_row", cell_lv, cell_row_lv, kZAxis, 10,
      rowParametrization, true); 
  
  // 4. Full sensor plane
  G4cout<<"Creating replacas of rows..." << G4endl; 
  SLArRTileParametrization* tplaneParametrization = 
    new SLArRTileParametrization(kXAxis, G4ThreeVector(-15*dx, 0, 0), 3*dx); 
  G4Box* cell_plane_box = new G4Box("tileCellPlane", 
      15*dx, 0.5*h, 15*dx); 
  G4LogicalVolume* cell_plane_lv = new G4LogicalVolume(cell_plane_box, 
      fMatReadoutTile->GetMaterial(), "rdtile_cell_plane_lv"); 
  cell_plane_lv->SetVisAttributes( G4VisAttributes(false) ); 
  //new G4PVReplica("cell_plane", cell_row_lv, cell_plane_lv, kXAxis, 10, 3*dx); 
  new G4PVParameterised("cell_plane", cell_row_lv, cell_plane_lv, kXAxis, 10, tplaneParametrization, true); 

  // 5. Final assembly (PCB + sensor plane)
  G4cout<<"Final placement..." << G4endl; 
  new G4PVPlacement(
      0, G4ThreeVector(0., 0.5*(fhTot-h), 0.), 
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

void SLArDetReadoutTile::BuildDefalutGeoParMap() 
{
  G4cout  << "SLArDetReadoutTile::BuildGeoParMap()" << G4endl;
  
  fGeoInfo->RegisterGeoPar("tile_z"   , 10.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("tile_x"   , 10.0*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("tile_y"   ,  2.5*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("sipm_z"   ,  6.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("sipm_x"   ,  6.0*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("sipm_y"   ,  1.8*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("pix_z"    ,  6.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("pix_x"    ,  6.0*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("pix_y"    ,  1.0*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("sipm_fill_factor",     0.9); 
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
  origin += fAxisVector*(copyNo+0.5)*fSpacing; 

  physVol->SetTranslation(origin); 
  physVol->SetRotation(0); 
  return; 
}

