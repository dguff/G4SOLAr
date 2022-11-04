/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetTPC.hh
 * @created     : giovedì nov 03, 2022 12:22:00 CET
 */

#ifndef SLARDETTPC_HH

#define SLARDETTPC_HH

#include "detector/SLArBaseDetModule.hh"
#include "detector/SuperCell/SLArDetSuperCell.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

struct SLArCryostatLayer{
  public:
    SLArCryostatLayer(); 
    SLArCryostatLayer(
        G4String   model_name, 
        G4double*  halfSize,  
        G4double   thickness,
        G4String   material_name);
    ~SLArCryostatLayer(); 

    G4String  fName;
    G4double  fHalfSizeX;
    G4double  fHalfSizeY; 
    G4double  fHalfSizeZ; 
    G4double  fThickness;

    G4String  fMaterialName;
    G4Material* fMaterial = nullptr;
    SLArBaseDetModule* fModule = nullptr; 
};

typedef std::map<int, SLArCryostatLayer*> SLArCryostatStructure; 

class SLArDetTPC : public SLArBaseDetModule
{

public:
  SLArDetTPC          ();
  virtual ~SLArDetTPC ();

  void          BuildCryostat();
  void          BuildTarget();

  void          BuildTPC();

  void          BuildMaterial(G4String);
  void          BuildDefalutGeoParMap();
  void          BuildCryostatStructure(const rapidjson::Value& jcryo); 

  void          SetVisAttributes();

  SLArBaseDetModule* GetTarget() {return fTarget;}
  SLArBaseDetModule* GetCryostat() {return fCryostat;}
  SLArCryostatStructure& GetCryostatStructure() {return fCryostatStructure;}

protected:
  SLArBaseDetModule* BuildCryostatLayer(G4String name, G4double x_, G4double y_, G4double z_, G4double tk, G4Material* mat); 

private:
  /* data */
  SLArBaseDetModule* fTarget;
  SLArBaseDetModule* fCryostat;
  SLArBaseDetModule* fWindow;
  G4VSolid*          fBoxOut;
  G4VSolid*          fBoxInn;
  G4VSolid*          fBoxTrg;

  // Some useful global variables
  SLArMaterial* fMatWorld;
  SLArMaterial* fMatSteel;
  SLArMaterial* fMatPlywood; 
  SLArMaterial* fMatPolyurethane; 
  SLArMaterial* fMatBPolyethilene; 
  SLArMaterial* fMatTarget;

  SLArCryostatStructure fCryostatStructure; 
};


#endif /* end of include guard SLARDETTPC_HH */
