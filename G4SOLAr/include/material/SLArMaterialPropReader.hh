/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialPropReader
 * @created     : mercoledì giu 19, 2019 16:55:20 CEST
 */

#ifndef SLArMATERIALPROPREADER_HH

#define SLArMATERIALPROPREADER_HH

#include "TString.h"
#include "TPRegexp.h"

#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4OpticalSurface.hh"

#include <fstream>
#include <iostream>

class SLArMPropVec {
  public:
    G4String fName    = "";
    G4MaterialPropertyVector*
             fVec ;
    SLArMPropVec() {
      fVec = new G4MaterialPropertyVector();
    }
};

class SLArMPropConst {
  public:
    G4String fName   = "";
    G4double fVal    = 0.;

    void FillValue(G4double val);
};

class SLArMaterialPropReader {
  public :
    SLArMaterialPropReader();
    SLArMaterialPropReader( G4String path );
    ~SLArMaterialPropReader();

    void SetPath( G4String  path ) { fPath = path; }
    void ReadFile(); 
    void PrintPropertySummary();

    std::vector<SLArMPropConst> fPropConst;
    std::vector<SLArMPropVec>   fPropVec  ;

    G4String                    fSurfName;
    G4OpticalSurfaceModel       fSurfModel;
    G4OpticalSurfaceFinish      fSurfFinish;
    G4SurfaceType               fSurfType;

  private :
    std::ifstream          fFile;
    G4String               fPath;

    G4bool                 IsReadingData = false;

    void                   ReadLineSurfaceDef (TString line);
    void                   ReadLineVec  (TString line);
    void                   ReadLineConst(TString line);
    G4double               ReadVal      (TString str );
    G4String               ReadStr      (TString str );

    TPRegexp rgxNum  = TPRegexp("\\s*\\d+(\\.\\d*)?([eE][+-]?\\d+)?");
    TPRegexp rgxPar  = TPRegexp("\\w+");
    TPRegexp rgxNist = TPRegexp("G4_\\w+");
    TPRegexp rgxUnit = TPRegexp("\\s*\\d+(\\.\\d*)?([eE][+-]?\\d+)?\\s+\\w+");
};



#endif /* end of include guard SLArMATERIALREADER_HH */

