/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArAbsModule
 * @created     : mercoledì ago 07, 2019 13:08:35 CEST
 */

#ifndef SLArABSMODULE_HH

#define SLArABSMODULE_HH

#include "material/SLArMaterial.hh"
#include "detector/SLArGeoInfo.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VSolid.hh"
#include "G4RotationMatrix.hh"


enum EPhotoDetPosition {kTop=0, kBottom=1};

class SLArBaseDetModule 
{
  public:
    SLArBaseDetModule();
    SLArBaseDetModule(const SLArBaseDetModule &base);
    virtual ~SLArBaseDetModule();

    SLArGeoInfo*         GetGeoInfo();
    G4bool             ContainsGeoPar(G4String str);
    void               SetGeoPar(G4String str, G4double val);
    void               SetGeoPar(std::pair<G4String, G4double> p);
    G4double           GetGeoPar(G4String str); 
    
    void               SetSolidVolume(G4VSolid* sol_vol);
    G4VSolid*          GetModSV();

    void               SetLogicVolume(G4LogicalVolume* log_vol);
    G4LogicalVolume*   GetModLV();

    void               SetRotation(G4RotationMatrix* rot);
    void               SetTranslation(G4ThreeVector* vec);

    void               SetMaterial(G4Material* mat);
    G4Material*        GetMaterial();

    G4RotationMatrix*  GetRotation();
    G4ThreeVector*     GetTranslation();

    void               ResetGeometry();

    void SetModPV(G4VPhysicalVolume* pv) {fModPV = pv;}
    G4VPhysicalVolume* GetModPV();
    G4VPhysicalVolume* GetModPV(
        G4String                          name, 
        G4RotationMatrix*                 rot,
        const G4ThreeVector               &vec,
        G4LogicalVolume*                  mlv,
        G4bool                            pMany = false,
        G4int                             pCopyNo = 0);

  protected:
    G4Material*        fMaterial;
    SLArGeoInfo*       fGeoInfo ;

    G4LogicalVolume*   fModLV   ;
    G4VSolid*          fModSV   ;
    G4VPhysicalVolume* fModPV   ;

    G4RotationMatrix*  fRot     ;
    G4ThreeVector      fVec     ;
    G4String           fName    ;
};


#endif /* end of include guard SLArABSMODULE_HH */

