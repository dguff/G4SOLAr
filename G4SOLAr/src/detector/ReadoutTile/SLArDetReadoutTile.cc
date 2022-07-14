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
#include "G4VPhysicalVolume.hh"

#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4VisAttributes.hh"
#include "G4MaterialPropertyVector.hh"

SLArDetReadoutTile::SLArDetReadoutTile() : SLArBaseDetModule(),
  fPerfectQE(false),
  fBasePCB(nullptr), fChargePix(nullptr), fSiPM(nullptr),
  fMatReadoutTile(nullptr), fMatPCB(nullptr), fMatSiPM(nullptr)
{  
  fGeoInfo = new SLArGeoInfo();
}

SLArDetReadoutTile::SLArDetReadoutTile(const SLArDetReadoutTile &detReadoutTile) 
  : SLArBaseDetModule(detReadoutTile)
{
  fPerfectQE   = detReadoutTile.fPerfectQE;
  fGeoInfo     = detReadoutTile.fGeoInfo;
  fMatReadoutTile= detReadoutTile.fMatReadoutTile;

  fMatReadoutTile = new SLArMaterialInfo(*detReadoutTile.fMatReadoutTile); 
  fMatPCB = new SLArMaterialInfo(*detReadoutTile.fMatPCB);
  fMatSiPM   = new SLArMaterialInfo(*detReadoutTile.fMatSiPM);

}

SLArDetReadoutTile::~SLArDetReadoutTile() {
  G4cerr << "Deleting SLArDetReadoutTile... " <<  G4endl;

  if (fBasePCB)   {delete fBasePCB; fBasePCB = 0;}
  if (fChargePix) {delete fChargePix; fChargePix = 0;}
  if (fSiPM)      {delete fSiPM; fSiPM = 0;}
  if (fMatReadoutTile) {delete fMatReadoutTile; fMatReadoutTile = 0;}
  if (fMatPCB){delete fMatPCB; fMatPCB = 0;}
  if (fMatSiPM)   {delete fMatSiPM; fMatSiPM = 0;}
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
    new G4Box("LightGuideSlab", 
        0.5*fBasePCB->GetGeoPar("tile_x"),
        0.5*fBasePCB->GetGeoPar("tile_y"),
        0.5*fBasePCB->GetGeoPar("tile_z"));

  fBasePCB->SetSolidVolume(lgbox);
  fBasePCB->SetLogicVolume(
    new G4LogicalVolume(fBasePCB->GetModSV(), 
      fBasePCB->GetMaterial(),
      "LightGuideLV", 0, 0, 0)
    );
}

void SLArDetReadoutTile::BuildSiPM()
{
  G4cout << "Building ReadoutTile SiPMs..." << G4endl;
  fSiPM = new SLArBaseDetModule();
  fSiPM->SetGeoPar(fGeoInfo->GetGeoPair("sipm_y"));
  fSiPM->SetGeoPar(fGeoInfo->GetGeoPair("sipm_x"));
  fSiPM->SetGeoPar(fGeoInfo->GetGeoPair("sipm_z"));

  fSiPM->SetMaterial(fMatSiPM->GetMaterial());

  fSiPM->SetSolidVolume(
        new G4Box("Coating", 
          0.5*fSiPM->GetGeoPar("sipm_x"),
          0.5*fSiPM->GetGeoPar("sipm_y"),
          0.5*fSiPM->GetGeoPar("sipm_z"))
        );
 
  fSiPM->SetLogicVolume(
      new G4LogicalVolume(fSiPM->GetModSV(), 
        fSiPM->GetMaterial(), "Coating", 0, 0, 0)
      );
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
        new G4Box("Coating", 
          0.5*fChargePix->GetGeoPar("pix_x"),
          0.5*fChargePix->GetGeoPar("pix_y"),
          0.5*fChargePix->GetGeoPar("pix_z"))
        );
 
  fChargePix->SetLogicVolume(
      new G4LogicalVolume(fChargePix->GetModSV(), 
        fChargePix->GetMaterial(), "Coating", 0, 0, 0)
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
      G4ThreeVector(0, h, 0),
      fModLV, false, 200);

  // 2. The SiPMs
  h = fhTot - fGeoInfo->GetGeoPar("sipm_y"); 
  G4cout<<"GetModPV SiPM..." << G4endl; 

  G4double dx = 3*CLHEP::mm;
  G4double sipm_x = fSiPM->GetGeoPar("sipm_x"); 
  G4double x_min = -0.5*fBasePCB->GetGeoPar("tile_x");
  G4double x_max = +0.5*fBasePCB->GetGeoPar("tile_x");
  G4double dz = 3*CLHEP::mm;
  G4double sipm_z = fSiPM->GetGeoPar("sipm_z");  
  G4double z_min = -0.5*fBasePCB->GetGeoPar("tile_z");
  G4double z_max = +0.5*fBasePCB->GetGeoPar("tile_z");

  G4double x = x_min + dx;
  

  while ( x < x_max - 0.5*sipm_x ) {
    G4double z = z_min + dz; 
    while (z < z_max - 0.5*sipm_z ) {
      fSiPM->GetModPV("ReadoutTileSiPM", 0, 
          G4ThreeVector(x , h, 0),
          fModLV, false, 200);
    }

  }


   return;
}

void SLArDetReadoutTile::ResetReadoutTileGeometry() 
{
  G4cout<< "Reset ReadoutTile Geometry" << G4endl;
  
  return; 
}

void SLArDetReadoutTile::SetVisAttributes()
{
  G4VisAttributes* visAttributes = new G4VisAttributes();
  visAttributes->SetColor(0.862, 0.952, 0.976, 0.5);
  fBasePCB->GetModLV()->SetVisAttributes( visAttributes );

  visAttributes = new G4VisAttributes( G4Color(0.968, 0.494, 0.007) );
  fSiPM->GetModLV()->SetVisAttributes( visAttributes );

  visAttributes = new G4VisAttributes( G4Color(0.968, 0.494, 0.007) );
  fChargePix->GetModLV()->SetVisAttributes( visAttributes );

  visAttributes = new G4VisAttributes();
  visAttributes->SetColor(0.305, 0.294, 0.345, 0.0);
  fModLV->SetVisAttributes( visAttributes );

  return;
}

SLArBaseDetModule* SLArDetReadoutTile::GetSiPM()
{
  return fCoating;
}


SLArMaterialInfo* SLArDetReadoutTile::GetSiPMMaterial()
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
    G4double phEne[2] = {1*CLHEP::eV, 5*CLHEP::eV};
    G4double eff  [2] = {1.0 , 1.0 };
    
    fMatSiPM->GetMaterialBuilder()->GetSurface()
               ->GetMaterialPropertiesTable()
               ->AddProperty("EFFICIENCY", phEne, eff, 2);  
  }

  return;
}

void SLArDetReadoutTile::BuildDefalutGeoParMap() 
{
  G4cout  << "SLArDetReadoutTile::BuildGeoParMap()" << G4endl;
  
  // Light guide and Coating layer size indicates:
  // * the side width 
  // * side length 
  // * light guide thickness
  // * coating layer thikness
  fGeoInfo->RegisterGeoPar("cell_z"   , 50.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("cell_x"   , 10.0*CLHEP::cm);
  fGeoInfo->RegisterGeoPar("coating_y",  0.5*CLHEP::mm);
  fGeoInfo->RegisterGeoPar("cell_y"   ,  4.0*CLHEP::mm);
}


void SLArDetReadoutTile::BuildMaterial()
{
  fMatPCB   = new SLArMaterialInfo();
  fMatSiPM      = new SLArMaterialInfo();
  fMatReadoutTile    = new SLArMaterialInfo();

  fMatReadoutTile->SetMaterialID("LAr");
  fMatReadoutTile->GetMaterialBuilder()->BuildMaterial();

  fMatPCB->SetMaterialID("Plastic");
  fMatPCB->GetMaterialBuilder()->BuildMaterial();

  fMatChargePix->SetMaterialID("Gold");
  fMatChargePix->GetMaterialBuilder()->BuildMaterial();

  fMatSiPM->SetMaterialID("Bialkali");
  fMatSiPM->GetMaterialBuilder()->BuildMaterial();
}


