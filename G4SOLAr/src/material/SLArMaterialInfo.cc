/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialInfo
 * @created     : giovedì ago 01, 2019 10:28:59 CEST
 */

#include "material/SLArMaterialInfo.h"

SLArMaterialInfo::SLArMaterialInfo()
{
  fMatBuilder = new SLArMaterialBuilder();
}

SLArMaterialInfo::SLArMaterialInfo(const SLArMaterialInfo &mat)
{
  fMatBuilder    = mat.fMatBuilder   ; 
  fMaterialID    = mat.fMaterialID   ; 
  fMaterial      = mat.fMaterial     ; 
  fMaterialTable = mat.fMaterialTable; 
  optSurf        = mat.optSurf       ; 
}

SLArMaterialInfo::SLArMaterialInfo(G4String matID)
{
  fMatBuilder = new SLArMaterialBuilder();
  SetMaterialID(matID);
}

SLArMaterialInfo::~SLArMaterialInfo()
{
  delete fMatBuilder;
}

G4Material* SLArMaterialInfo::GetMaterial() 
{
  return fMaterial;
}

G4Material* SLArMaterialInfo::GetMaterialTable()
{
  return fMaterialTable;
}

void SLArMaterialInfo::SetMaterialID(G4String matID)
{
  fMaterialID = matID;
  fMatBuilder->InitBuilder(fMaterialID);
  fMatBuilder->BuildMaterial();

  fMaterial = fMatBuilder->GetMaterial();
}

G4String SLArMaterialInfo::GetMaterialID()
{
  return fMaterialID;
}
