/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArDetectorSize
 * @created     : giovedì giu 20, 2019 09:52:50 CEST
 */

#include "detector/SLArDetectorSize.hh"
#include "material/SLArMaterialBuilder.hh"

#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"

SLArDetectorElement::SLArDetectorElement() {
  fMatBuilder = new SLArMaterialBuilder();
}

SLArDetectorElement::SLArDetectorElement(G4String mat) {
  fMaterialID = mat;
  SLArDetectorElement();
}

SLArDetectorElement::~SLArDetectorElement() {
  delete fMatBuilder;
}

