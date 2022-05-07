/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialInfo
 * @created     : giovedì ago 01, 2019 10:05:56 CEST
 */

#ifndef SLArMATERIALINFO_H

#define SLArMATERIALINFO_H

#include "SLArMaterialBuilder.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"

class SLArMaterialInfo {
  public:
    SLArMaterialInfo();
    SLArMaterialInfo(const SLArMaterialInfo &mat);
    SLArMaterialInfo(G4String matID);
    ~SLArMaterialInfo();

    SLArMaterialBuilder* GetMaterialBuilder() {return fMatBuilder;}
    G4Material*          GetMaterial();
    G4Material*          GetMaterialTable();
    G4String             GetMaterialID();
    void                 SetMaterialID(G4String matID);

  protected:
    SLArMaterialBuilder* fMatBuilder;

  private:
    G4String           fMaterialID    = ""     ;
    G4Material*        fMaterial      = nullptr;
    G4Material*        fMaterialTable = nullptr;
    G4Material*        optSurf        = nullptr;
};


#endif /* end of include guard SLArMATERIALINFO_H */

