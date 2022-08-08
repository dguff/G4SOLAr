/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetReadoutPlane
 * @created     : lunedì lug 18, 2022 11:10:23 CEST
 */

#include "detector/ReadoutTile/SLArDetReadoutPlane.hh"
#include "detector/ReadoutTile/SLArDetReadoutTile.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4VPhysicalVolume.hh"

#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4VisAttributes.hh"
#include "G4MaterialPropertyVector.hh"

SLArDetReadoutPlane::SLArDetReadoutPlane() 
  : SLArBaseDetModule(), fMatReadoutPlane(nullptr), fTileRow(nullptr)
{
  fGeoInfo = new SLArGeoInfo();  
}

SLArDetReadoutPlane::SLArDetReadoutPlane(const SLArDetReadoutPlane& detReadoutPlane) 
  : SLArBaseDetModule(detReadoutPlane), fMatReadoutPlane(nullptr), fTileRow(nullptr) 
{
  fMatReadoutPlane = new SLArMaterial(*detReadoutPlane.fMatReadoutPlane);
}

SLArDetReadoutPlane::~SLArDetReadoutPlane()
{}

void SLArDetReadoutPlane::BuildDefalutGeoParMap() 
{
  G4cout  << "SLArDetReadoutPlane::BuildGeoParMap()" << G4endl;
  
  fGeoInfo->RegisterGeoPar("rdoutplane_z"   ,  1.0*CLHEP::m);
  fGeoInfo->RegisterGeoPar("rdoutplane_x"   ,  2.0*CLHEP::m);
}

void SLArDetReadoutPlane::BuildMaterial()
{
  fMatReadoutPlane = new SLArMaterial(); 
  fMatReadoutPlane->SetMaterialID("LAr");
  fMatReadoutPlane->BuildMaterialFromDB();
}

void SLArDetReadoutPlane::BuildTileRow(SLArDetReadoutTile* tile) {
  fTileRow = new SLArBaseDetModule();
  fTileRow->SetMaterial(fMatReadoutPlane->GetMaterial());
  G4double tile_x  = tile->GetGeoPar("tile_x"); 
  G4double tile_y  = tile->GetGeoPar("tile_y") + tile->GetGeoPar("sipm_y");
  G4double tile_z  = tile->GetGeoPar("tile_z"); 
  G4double plane_z = fGeoInfo->GetGeoPar("rdoutplane_z"); 

  G4int n_z = std::floor(plane_z / tile_z); 

  G4double true_plane_z = tile_z*n_z;
  
  G4Box* tile_row_z_box = new G4Box("tile_row_z_box", 
      0.5*tile_x, 0.5*tile_y, 0.5*true_plane_z); 
  fTileRow->SetSolidVolume(tile_row_z_box);
  fTileRow->SetLogicVolume(
      new G4LogicalVolume(tile_row_z_box, 
      fMatReadoutPlane->GetMaterial(), "tile_row_z_lv") 
      );
  fTileRow->GetModLV()->SetVisAttributes( G4VisAttributes(false) ); 
  
  fTileRow->SetModPV(
      new G4PVReplica("tile_row_z", tile->fModLV, fTileRow->GetModLV(), kZAxis, n_z, tile_z)
  );
  fTileRow->SetGeoPar("tilerow_x", tile_x);
  fTileRow->SetGeoPar("tilerow_y", tile_y);
  fTileRow->SetGeoPar("tilerow_z", true_plane_z);
}

void SLArDetReadoutPlane::BuildReadoutPlane(SLArDetReadoutTile* tile) 
{
  G4double tile_x  = tile->GetGeoPar("tile_x"); 
  G4double tile_y  = tile->GetGeoPar("tile_y") + tile->GetGeoPar("sipm_y");
  G4double tile_z  = tile->GetGeoPar("tile_z"); 
  G4double plane_x = fGeoInfo->GetGeoPar("rdoutplane_x"); 
  G4double plane_z = fGeoInfo->GetGeoPar("rdoutplane_z"); 
  G4double plane_y = tile_y; 

  G4int n_z = std::floor(plane_z / tile_z); 
  G4int n_x = std::floor(plane_x / tile_x); 

  G4double true_plane_x = tile_x*n_x;
  G4double true_plane_z = tile_z*n_z;
  
  BuildTileRow(tile);

  G4VSolid* ReadoutPlane_box = new G4Box("ReadoutPlane_box", 
      true_plane_x*0.5, plane_y*0.5, true_plane_z*0.5);
  fModLV = new G4LogicalVolume(ReadoutPlane_box, 
      fMatReadoutPlane->GetMaterial(), "ReadoutPlaneLV", 0, 0, 0, 1); 
  fModLV->SetVisAttributes( G4VisAttributes(false) ); 
  SetModPV(
      new G4PVReplica("ReadoutPlane", fTileRow->GetModLV(), fModLV, kXAxis, n_x, tile_x)
  );
}


