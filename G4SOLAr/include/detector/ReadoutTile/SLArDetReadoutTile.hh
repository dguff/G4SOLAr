/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetReadoutTile.hh
 * @created     : giovedì lug 14, 2022 09:34:31 CEST
 */

#ifndef SLARDETREADOUTTILE_HH

#define SLARDETREADOUTTILE_HH

#include "detector/SLArBaseDetModule.hh"
#include "material/SLArMaterialInfo.h"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

class SLArDetReadoutTile : public SLArBaseDetModule
{

public:
  SLArDetReadoutTile            ();
  SLArDetReadoutTile            (const SLArDetReadoutTile &detReadoutTile);
  ~SLArDetReadoutTile ();
  
  void          SetPerfectQE(G4bool kQE);

  void          BuildMaterial();
  void          BuildDefalutGeoParMap();
  void          BuildReadoutTile();
  void          BuildPCB();
  void          BuildSiPM();
  void          BuildChargePix();
  void          ResetReadoutTileGeometry();
  void          SetVisAttributes();

  SLArBaseDetModule*   GetSiPM();
  SLArMaterialInfo*    GetSiPMMaterial();


protected:

private:
  G4double                fhTot;
  G4double                fSize;

  G4bool             fPerfectQE;

  SLArBaseDetModule* fBasePCB;
  SLArBaseDetModule* fChargePix;
  SLArBaseDetModule* fSiPM; 

  SLArMaterialInfo*  fMatReadoutTile; 
  SLArMaterialInfo*  fMatPCB;
  SLArMaterialInfo*  fMatChargePix;
  SLArMaterialInfo*  fMatSiPM; 

  friend class SLArDetReadoutPlane;
};

#endif /* end of include guard SLARDETREADOUTTILE_HH */

