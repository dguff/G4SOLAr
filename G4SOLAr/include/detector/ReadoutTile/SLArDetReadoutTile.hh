/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetReadoutTile.hh
 * @created     : giovedì lug 14, 2022 09:34:31 CEST
 */

#ifndef SLARDETREADOUTTILE_HH

#define SLARDETREADOUTTILE_HH

#include "detector/SLArBaseDetModule.hh"
#include "G4VPVParameterisation.hh"
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
  void          SetVisAttributes();

  SLArBaseDetModule*       GetSiPMActive();
  SLArMaterial*    GetSiPMActiveMaterial();
  G4LogicalSkinSurface*    GetSiPMLgSkin() {return fSkinSurface;}


protected:

private:
  G4double                fhTot;
  G4double                fSize;

  G4bool             fPerfectQE;

  SLArBaseDetModule* fBasePCB;
  SLArBaseDetModule* fChargePix;
  SLArBaseDetModule* fSiPM;
  SLArBaseDetModule* fSiPMActive; 

  SLArMaterial*  fMatReadoutTile; 
  SLArMaterial*  fMatPCB;
  SLArMaterial*  fMatChargePix;
  SLArMaterial*  fMatSiPM; 
  SLArMaterial*  fMatSiPMCapsule;

  G4LogicalSkinSurface* fSkinSurface;

  friend class SLArDetReadoutPlane;

public: 
  class SLArRTileParametrization : public G4VPVParameterisation {
    public: 
      SLArRTileParametrization(EAxis, G4ThreeVector, G4double);

      void ComputeTransformation(G4int copyNo, G4VPhysicalVolume* physVol) const; 

    private: 
      EAxis fReplicaAxis; 
      G4ThreeVector fAxisVector; 
      G4ThreeVector fStartPos; 
      G4double  fSpacing; 

  };
};


#endif /* end of include guard SLARDETREADOUTTILE_HH */

