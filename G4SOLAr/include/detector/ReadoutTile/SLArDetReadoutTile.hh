/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetReadoutTile.hh
 * @created     : giovedì lug 14, 2022 09:34:31 CEST
 */

#ifndef SLARDETREADOUTTILE_HH

#define SLARDETREADOUTTILE_HH

#include "detector/SLArBaseDetModule.hh"
#include "G4LogicalSkinSurface.hh"


class SLArDetReadoutTile : public SLArBaseDetModule
{

public:
  SLArDetReadoutTile            ();
  SLArDetReadoutTile            (const SLArDetReadoutTile &detReadoutTile);
  ~SLArDetReadoutTile ();
  
  void          SetPerfectQE(G4bool kQE);

  void          BuildMaterial(G4String materials_db);
  void          BuildDefalutGeoParMap();
  G4LogicalSkinSurface* BuildLogicalSkinSurface(); 
  void          BuildReadoutTile();
  void          BuildPCB();
  void          BuildSiPM();
  void          BuildChargePix();
  void          ResetReadoutTileGeometry();
  void          SetVisAttributes();

  SLArBaseDetModule*       GetSiPM();
  SLArMaterial*    GetSiPMMaterial();
  G4LogicalSkinSurface* GetSiPMLgSkin() {return fSkinSurface;}


protected:

private:
  G4double                fhTot;
  G4double                fSize;

  G4bool             fPerfectQE;

  SLArBaseDetModule* fBasePCB;
  SLArBaseDetModule* fChargePix;
  SLArBaseDetModule* fSiPM; 

  SLArMaterial*  fMatReadoutTile; 
  SLArMaterial*  fMatPCB;
  SLArMaterial*  fMatChargePix;
  SLArMaterial*  fMatSiPM; 

  G4LogicalSkinSurface* fSkinSurface;

  friend class SLArDetReadoutPlane;
};

#endif /* end of include guard SLARDETREADOUTTILE_HH */

