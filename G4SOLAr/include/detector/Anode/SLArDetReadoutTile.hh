/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetReadoutTile.hh
 * @created     : giovedì lug 14, 2022 09:34:31 CEST
 */

#ifndef SLARDETREADOUTTILE_HH

#define SLARDETREADOUTTILE_HH

#include "detector/SLArBaseDetModule.hh"
#include "G4LogicalSkinSurface.hh"

class TH2Poly; 

class SLArDetReadoutTile : public SLArBaseDetModule
{

struct SUnitCellComponent {
  G4String fName; 
  SLArBaseDetModule* fMod; 
  G4ThreeVector fPos;
  G4int fCopyNo;

  SUnitCellComponent(G4String, G4int, SLArBaseDetModule*, G4ThreeVector); 
}; 

struct SUnitCellPixelArea {
  G4String fName; 
  std::vector<G4ThreeVector> fEdges; 

  SUnitCellPixelArea(G4String); 
}; 

public:
  SLArDetReadoutTile();
  SLArDetReadoutTile(const SLArDetReadoutTile &detReadoutTile);
  ~SLArDetReadoutTile();
  
  void SetPerfectQE(G4bool kQE);

  void BuildMaterial(G4String materials_db);
  void BuildComponentsDefinition(const rapidjson::Value&); 
  void BuildUnitCellPixMap(const rapidjson::Value&); 
  void BuildUnitCellStructure(const rapidjson::Value&); 
  G4LogicalSkinSurface* BuildLogicalSkinSurface(); 
  void BuildReadoutTile();
  void BuildPCB();
  void BuildSiPM();
  TH2Poly* BuildTileChgPixelMap(const G4ThreeVector& xAxis, const G4ThreeVector& yAxis, 
      const G4ThreeVector* _shift = nullptr, const G4RotationMatrix* _rot = nullptr); 
  void BuildChargePix();
  void BuildUnitCell(); 

  void SetVisAttributes(const int depth = 0);

  SLArBaseDetModule* GetSiPMActive();
  SLArBaseDetModule* GetUnitCell() {return fUnitCell;}
  SLArMaterial* GetSiPMActiveMaterial();
  SLArBaseDetModule* GetChargePixel() {return fChargePix;}
  G4LogicalSkinSurface* GetSiPMLgSkin() {return fSkinSurface;}
  virtual void Init(const rapidjson::Value&) override {}
  const std::vector<SUnitCellComponent>& GetUnitCellStructure() {return fCellStructure;}
  const std::vector<SUnitCellPixelArea>& GetUnitCellPixelMap() {return fCellPixelMap;}
protected:

private:
  G4double                fhTot;
  G4double                fSize;

  G4bool             fPerfectQE;

  SLArBaseDetModule* fBasePCB;
  SLArBaseDetModule* fBaseTile;
  SLArBaseDetModule* fChargePix;
  SLArBaseDetModule* fSiPM;
  SLArBaseDetModule* fSiPMActive; 
  SLArBaseDetModule* fUnitCell; 

  SLArMaterial*  fMatReadoutTile; 
  SLArMaterial*  fMatPCB;
  SLArMaterial*  fMatCopper; 
  SLArMaterial*  fMatChargePix;
  SLArMaterial*  fMatSiPM; 
  SLArMaterial*  fMatSiPMCapsule;
  G4LogicalSkinSurface* fSkinSurface;

  std::vector<SUnitCellComponent> fCellStructure; 
  std::vector<SUnitCellPixelArea> fCellPixelMap; 

  friend class SLArDetReadoutPlane;

public: 
};

inline SLArDetReadoutTile::SUnitCellComponent::SUnitCellComponent(
    G4String name, G4int copyNo, SLArBaseDetModule* mod, G4ThreeVector pos) {
 fName = name; 
 fCopyNo = copyNo; 
 fMod = mod; 
 fPos = pos; 
}

inline SLArDetReadoutTile::SUnitCellPixelArea::SUnitCellPixelArea(G4String name) 
  : fName(name) {}

#endif /* end of include guard SLARDETREADOUTTILE_HH */

