/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArMaterialInfo.hh
 * @created     : lunedì ago 08, 2022 17:45:44 CEST
 */

#ifndef SLARMATERIALINFO_H

#define SLARMATERIALINFO_H

#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"

class SLArMaterial {
  public:
    SLArMaterial();
    SLArMaterial(const SLArMaterial &mat);
    SLArMaterial(G4String matID);
    ~SLArMaterial();

    void                BuildMaterialFromDB(G4String mat_id = "");
    G4Material*         GetMaterial();
    G4MaterialPropertiesTable* GetMaterialPropTable(); 
    G4String            GetMaterialID();
    void                SetMaterialID(G4String matID) {fMaterialID = matID;}

  protected:

  private:
    G4String            fMaterialID;
    G4Material*         fMaterial  ;
    G4Material*         optSurf    ;
};


#endif /* end of include guard SLArMATERIAL_H */

