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

struct SUnitCellStruct {
  G4String fName; 
  SLArBaseDetModule* fMod; 
  G4ThreeVector fPos;
  G4int fCopyNo;

  SUnitCellStruct(G4String, G4int, SLArBaseDetModule*, G4ThreeVector); 
}; 

public:
  SLArDetReadoutTile();
  SLArDetReadoutTile(const SLArDetReadoutTile &detReadoutTile);
  ~SLArDetReadoutTile();
  
  void SetPerfectQE(G4bool kQE);

  void BuildMaterial(G4String materials_db);
  void BuildComponentsDefinition(const rapidjson::Value&); 
  void BuildUnitCellStructure(const rapidjson::Value&); 
  G4LogicalSkinSurface* BuildLogicalSkinSurface(); 
  void BuildReadoutTile();
  void BuildPCB();
  void BuildSiPM();
  void BuildChargePix();
  void BuildUnitCell(); 
  void SetVisAttributes();

  SLArBaseDetModule* GetSiPMActive();
  SLArBaseDetModule* GetUnitCell() {return fUnitCell;}
  SLArMaterial* GetSiPMActiveMaterial();
  G4LogicalSkinSurface* GetSiPMLgSkin() {return fSkinSurface;}



protected:

private:
  G4double                fhTot;
  G4double                fSize;

  G4bool             fPerfectQE;

  SLArBaseDetModule* fBasePCB;
  SLArBaseDetModule* fChargePix;
  SLArBaseDetModule* fSiPM;
  SLArBaseDetModule* fSiPMActive; 
  SLArBaseDetModule* fUnitCell; 

  SLArMaterial*  fMatReadoutTile; 
  SLArMaterial*  fMatPCB;
  SLArMaterial*  fMatChargePix;
  SLArMaterial*  fMatSiPM; 
  SLArMaterial*  fMatSiPMCapsule;
  G4LogicalSkinSurface* fSkinSurface;

  std::vector<SUnitCellStruct> fCellStructure; 

  friend class SLArDetReadoutPlane;

public: 
  class SLArRTileParametrization : public G4VPVParameterisation {
    public: 
      SLArRTileParametrization(EAxis, G4ThreeVector, G4double);

      void ComputeTransformation(G4int copyNo, G4VPhysicalVolume* physVol) const; 

      EAxis GetReplicationAxis() {return fReplicaAxis;}
      G4double GetSpacing() {return fSpacing;}
      G4ThreeVector GetStartPos() {return fStartPos;}


    private: 
      EAxis fReplicaAxis; 
      G4ThreeVector fAxisVector; 
      G4ThreeVector fStartPos; 
      G4double  fSpacing; 

  };
};

inline SLArDetReadoutTile::SUnitCellStruct::SUnitCellStruct(
    G4String name, G4int copyNo, SLArBaseDetModule* mod, G4ThreeVector pos) {
 fName = name; 
 fCopyNo = copyNo; 
 fMod = mod; 
 fPos = pos; 
}

#endif /* end of include guard SLARDETREADOUTTILE_HH */

