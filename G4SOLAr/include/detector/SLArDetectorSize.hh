/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArDetectorSize
 * @created     : mercoledì giu 19, 2019 09:09:48 CEST
 */

#ifndef SLArDETECTORSIZE_HH

#define SLArDETECTORSIZE_HH

#include "material/SLArMaterialBuilder.hh"

#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"

#include <ostream>
#include <map>

class SLArDetectorElement {

  protected:
    SLArMaterialBuilder* fMatBuilder;
    void               BuildMaterial();
    void               BuildMaterialTable();
    void               BuildSufaceTable();

  public:
    SLArDetectorElement();
    SLArDetectorElement(G4String mat);
    ~SLArDetectorElement();


    std::map<G4String, G4double> fGeoPar;
    G4String    fMaterialID    = ""     ;
    G4Material* fMaterial      = nullptr;
    G4Material* fMaterialTable = nullptr;
    G4Material* optSurf        = nullptr;
};

class SLArDetectorSize {
    class SLArGEOworld : public SLArDetectorElement {
      public:
        SLArGEOworld() : SLArDetectorElement() {
          fGeoPar.insert( std::pair<G4String, G4double> ("x", 1.0*m) );
          fGeoPar.insert( std::pair<G4String, G4double> ("y", 1.0*m) );
          fGeoPar.insert( std::pair<G4String, G4double> ("z", 1.0*m) );

          fMaterialID = "Air";

          fMatBuilder->InitBuilder(fMaterialID);
          fMatBuilder->BuildMaterial();
          fMaterial = fMatBuilder->GetMaterial();
        }
    };

    class SLArGEObox : public SLArDetectorElement {
      public:
        SLArGEObox() : SLArDetectorElement() {
          fGeoPar.insert( std::pair<G4String, G4double> ("x"    , 0.3* m) );
          fGeoPar.insert( std::pair<G4String, G4double> ("y"    , 0.3* m) );
          fGeoPar.insert( std::pair<G4String, G4double> ("z"    , 0.3* m) );
          fGeoPar.insert( std::pair<G4String, G4double> ("thick", 2.0*cm) );
          

          fMaterialID = "Steel";

          fMatBuilder->InitBuilder(fMaterialID);
          fMatBuilder->BuildMaterial();
          fMaterial = fMatBuilder->GetMaterial();
        }
    };


    class SLArGEOtarget : public SLArDetectorElement {
      public:
        SLArGEOtarget() : SLArDetectorElement() {
          fGeoPar.insert( std::pair<G4String, G4double> ("r", 10.0*cm) );
          fGeoPar.insert( std::pair<G4String, G4double> ("y",  2.0*cm) );

          fMaterialID = "LAr";
          fMatBuilder->InitBuilder(fMaterialID);
          fMatBuilder->BuildMaterial();
          fMaterial = fMatBuilder->GetMaterial();
        }
    };

    class SLArGEOlappd : public SLArDetectorElement {
      public:
        SLArGEOlappd() : SLArDetectorElement() {
          fGeoPar.insert( std::pair<G4String, G4double> ("x", 10.0*cm) );
          fGeoPar.insert( std::pair<G4String, G4double> ("y",  0.5*cm) );
          fGeoPar.insert( std::pair<G4String, G4double> ("z", 10.0*cm) );
          fGeoPar.insert( std::pair<G4String, G4double> ("shift", 3.0*cm) );

          fMaterialID = "Silicon";
          fMatBuilder->InitBuilder(fMaterialID);
          fMatBuilder->BuildMaterial();
          fMaterial = fMatBuilder->GetMaterial();
        }
    };

    class SLArGEOpmt : public SLArDetectorElement {
      public:
        SLArGEOpmt() : SLArDetectorElement() {
          fGeoPar.insert( std::pair<G4String, G4double> ("side"  , 3.2*cm) );
          fGeoPar.insert( std::pair<G4String, G4double> ("rTube" , 0.8   ) ); // wrt to side
          fGeoPar.insert( std::pair<G4String, G4double> ("hTube" , 0.8   ) ); // wrt to side
          fGeoPar.insert( std::pair<G4String, G4double> ("hGlass", 0.25  ) ); // wrt to side
          fGeoPar.insert( std::pair<G4String, G4double> ("hDyno" , 0.33  ) ); // wrt to side
          
          fMaterialID = "Al";
          fMatBuilder->InitBuilder(fMaterialID);
          fMatBuilder->BuildMaterial();
          fMaterial = fMatBuilder->GetMaterial();
        }
    };

    class SLArGEOhodo : public SLArDetectorElement {
      public:
        SLArGEOhodo() : SLArDetectorElement() {
          fGeoPar.insert( std::pair<G4String, G4double> ("HodoBarLength"  , 0.5*30.0*cm) );
          fGeoPar.insert( std::pair<G4String, G4double> ("HodoBarHeight"  , 0.5* 3.0*cm) );
          fGeoPar.insert( std::pair<G4String, G4double> ("HodoBarWidth"   , 0.5* 1.0*cm) );
          
          fMaterialID = "PLSCINT";
          fMatBuilder->InitBuilder(fMaterialID);
          fMatBuilder->BuildMaterial();
          fMaterial = fMatBuilder->GetMaterial();
        }
    };


  public:
    class SLArGEOcoll  {
      public:
        SLArGEOworld  fGEOWorld;
        SLArGEObox    fGEOBox;
        SLArGEOtarget fGEOTarget;
        SLArGEOlappd  fGEOLappd;
        SLArGEOpmt    fGEOPmt;
        SLArGEOhodo   fGeoHodo;
    };

    SLArGEOcoll fGEOColl;

};

#endif /* end of include guard SLArDETECTORSIZE_HH */

