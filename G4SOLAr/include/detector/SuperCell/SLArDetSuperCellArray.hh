/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetSuperCellArray.hh
 * @created     Fri Mar 24, 2023 15:57:35 CET
 */

#ifndef SLARDETSUPERCELLARRAY_HH

#define SLARDETSUPERCELLARRAY_HH

#include "detector/SLArBaseDetModule.hh"
#include "detector/SuperCell/SLArDetSuperCell.hh"
#include "detector/SLArPlaneParameterisation.hpp"

class SLArCfgSuperCellArray;

class SLArDetSuperCellArray : public SLArBaseDetModule {
  public:
    SLArDetSuperCellArray(); 
    ~SLArDetSuperCellArray(); 
    
    SLArCfgSuperCellArray BuildSuperCellArrayCfg(); 
    void BuildMaterial(G4String materials_db); 
    void BuildSuperCellArray(SLArDetSuperCell*); 

    const G4ThreeVector& GetNormal() {return fNormal;}
    const G4ThreeVector& GetPosition() {return fPosition;}
    const G4ThreeVector& GetGlbPosition() {return fGlobalPosition;}
    G4RotationMatrix* GetRotation() {return fRotation;}
    G4String GetPhotoDetModel() {return fPhotoDetModel;}
    G4int GetTPCID() {return fTPCID;}

    virtual void SetGlobalPos( const G4ThreeVector glb_pos ) {fGlobalPosition = glb_pos;}
    virtual void Init(const rapidjson::Value&) override; 

  private:
    std::pair<int, G4double> ComputeArrayTrueLength(G4double width, G4double spacing, G4double max_len);

    G4int fTPCID; 
    SLArMaterial* fMaterialBase;
    SLArBaseDetModule* fSuperCell; 
    G4ThreeVector fPosition; 
    G4ThreeVector fGlobalPosition; 
    G4ThreeVector fNormal;
    G4RotationMatrix* fRotation;
    G4String fPhotoDetModel;
    std::vector<SLArPlaneParameterisation*> fParameterisation; 
    std::vector<SLArBaseDetModule*> fSubModules;

}; 



#endif /* end of include guard SLARDETSUPERCELLARRAY_HH */

