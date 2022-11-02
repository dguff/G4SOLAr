/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArDetTank
 * @created     : mercoledì lug 31, 2019 15:06:12 CEST
 */

#ifndef SLArDETTANK_HH

#define SLArDETTANK_HH

#include "detector/SLArBaseDetModule.hh"
#include "detector/SuperCell/SLArDetSuperCell.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"


class SLArDetTank : public SLArBaseDetModule
{

public:
  SLArDetTank          ();
  virtual ~SLArDetTank ();

  void          BuildCryostat();
  void          BuildTarget();

  void          BuildTPC();

  void          BuildMaterial(G4String);
  void          BuildDefalutGeoParMap();

  void          SetVisAttributes();

  SLArBaseDetModule* GetTarget() {return fTarget;}
  SLArBaseDetModule* GetCryostat() {return fCryostat;}

protected:
  SLArBaseDetModule* BuildCryostatLayer(G4String name, G4double x_, G4double y_, G4double z_, G4double tk, SLArMaterial* mat); 

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
};

struct SLArCryostatLayer{
  public:
    SLArCryostatLayer(
        G4String   model_name, 
        G4double*  halfSize,  
        G4double   thickness,
        SLArMaterial* material)
    {
      fName  = model_name;
      fHalfSizeX =  halfSize[0];
      fHalfSizeY =  halfSize[1]; 
      fHalfSizeZ =  halfSize[2]; 
      fThickness =  thickness; 

      fMaterial = material;
    }

    G4String  fName;
    G4double  fHalfSizeX;
    G4double  fHalfSizeY; 
    G4double  fHalfSizeZ; 
    G4double  fThickness;

    SLArMaterial* fMaterial; 
};
#endif /* end of include guard SLArDETTANK_HH */

