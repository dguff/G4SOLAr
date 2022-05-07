/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArDetTank
 * @created     : mercoledì lug 31, 2019 15:06:12 CEST
 */

#ifndef SLArDETTANK_HH

#define SLArDETTANK_HH

#include "material/SLArMaterialInfo.h"

#include "detector/SLArBaseDetModule.hh"
//#include "detector/PMT/SLArDetPMT.hh"
#include "detector/Tank/SLArDetTankMsgr.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"


class SLArDetTank : public SLArBaseDetModule
{

public:
  SLArDetTank          ();
  virtual ~SLArDetTank ();

  void          BuildVessel();
  void          BuildTarget();

  void          BuildTank();

  void          BuildMaterial();
  void          BuildDefalutGeoParMap();

  void          ResetTankGeometry();

  void          SetVisAttributes();

  SLArBaseDetModule* GetTarget() {return fTarget;}
  SLArBaseDetModule* GetVessel() {return fVessel;}

protected:

private:
  /* data */
  SLArBaseDetModule* fTarget;
  SLArBaseDetModule* fVessel;
  SLArBaseDetModule* fWindow;
  G4VSolid*        fBoxOut;
  G4VSolid*        fBoxInn;
  G4VSolid*        fBoxTrg;

  // Some useful global variables
  SLArMaterialInfo* fMatWorld;
  SLArMaterialInfo* fMatVessel;
  SLArMaterialInfo* fMatTarget;
};

struct SLArExtrusion{
  public:
    SLArExtrusion(
        G4String  model_name, 
        G4VSolid* shape, 
        G4double  shift)
    {
      fName  = model_name;
      fShape = shape;
      fShift = shift;
    }
    G4String  GetName()  {return fName;}
    G4VSolid* GetShape() {return fShape;}
    G4double  GetShift() {return fShift;}
  private:
    G4String  fName;
    G4VSolid* fShape;
    G4double  fShift;
};
#endif /* end of include guard SLArDETTANK_HH */

