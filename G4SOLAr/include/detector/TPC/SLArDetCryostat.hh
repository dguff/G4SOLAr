/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetCryostat.hh
 * @created     Wed Mar 15, 2023 11:57:11 CET
 */

#ifndef SLARDETCRYOSTAT_HH

#define SLARDETCRYOSTAT_HH

#include "detector/SLArBaseDetModule.hh"
#include "detector/SLArGeoUtils.hpp"

struct SLArCryostatLayer{
  public:
    SLArCryostatLayer(); 
    SLArCryostatLayer(
        G4String   model_name, 
        G4double*  halfSize,  
        G4double   thickness,
        G4String   material_name,
        G4int      importance = 1);
    ~SLArCryostatLayer() {} 

    G4String  fName;
    G4double  fHalfSizeX;
    G4double  fHalfSizeY; 
    G4double  fHalfSizeZ; 
    G4double  fThickness;
    G4int     fImportance; 

    G4String  fMaterialName;
    G4Material* fMaterial = nullptr;
    SLArBaseDetModule* fModule = nullptr; 
};

typedef std::map<int, SLArCryostatLayer*> SLArCryostatStructure; 

class SLArDetCryostat : public SLArBaseDetModule {
  public:
    SLArDetCryostat(); 
    ~SLArDetCryostat(); 

    void BuildCryostat(); 
    void BuildMaterials(G4String); 
    void BuildCryostatStructure(const rapidjson::Value& jcryo);
    SLArCryostatStructure& GetCryostatStructure() {return fCryostatStructure;}
    std::map<slargeo::EBoxFace, SLArBaseDetModule*>& GetCryostatSupportStructure() {return fSupportStructure;}
    SLArBaseDetModule* GetWaffleUnit() {return fWaffleUnit;}
    virtual void Init(const rapidjson::Value&) override {}
    void SetWorldMaterial(SLArMaterial* mat) {fMatWorld = mat;}
    void SetVisAttributes();

  private: 
    SLArMaterial* fMatWorld; 
    SLArMaterial* fMatWaffle; 
    SLArBaseDetModule* fWaffleUnit;
    G4bool fBuildSupport; 
    std::map<G4String, SLArMaterial*> fMaterials;
    std::map<slargeo::EBoxFace, SLArBaseDetModule*> fSupportStructure;

    SLArCryostatStructure fCryostatStructure; 
    SLArBaseDetModule* BuildCryostatLayer(
        G4String name, 
        G4double x_, G4double y_, G4double z_, G4double tk_, 
        G4Material* mat);
    void BuildSupportStructureUnit(); 
    SLArBaseDetModule* BuildSupportStructure(slargeo::EBoxFace kFace); 

};




#endif /* end of include guard SLARDETCRYOSTAT_HH */

