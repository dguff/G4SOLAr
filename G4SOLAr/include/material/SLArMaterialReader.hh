/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialReader
 * @created     : mercoledì giu 19, 2019 16:55:20 CEST
 */

#ifndef SLArMATERIALREADER_HH

#define SLArMATERIALREADER_HH

#include "TString.h"
#include "TPRegexp.h"

#include "G4Material.hh"
#include "G4OpticalSurface.hh"

#include <fstream>
#include <iostream>

class SLArElement {
  public:
    G4String fName    = "";
    G4String fNistName= "";
    G4String fSymb    = "";
    G4int    fZ       = 0 ;
    G4int    fNAtom   = 0 ;
    G4double fMolMass = 0*CLHEP::g/CLHEP::mole;
    G4double fFraction= 0 ;

    void PrintElementSummary();
};

class SLArMaterialReader {
  public :
    SLArMaterialReader();
    SLArMaterialReader( G4String name );
    SLArMaterialReader( G4String name, G4String path );
    ~SLArMaterialReader();

    void SetPath( G4String  path ) { fPath = path; }
    void LoadInfo();
    void PrintMaterialSummary();

    G4String               fName;
    G4String               fNistName;
    G4double               fDensity;
    G4int                  fNComp;
    std::vector<SLArElement> fElVec;

    G4String               fSurfName;
    G4OpticalSurfaceModel  fSurfModel;
    G4SurfaceType          fSurfType;
    G4OpticalSurfaceFinish fSurfFinish;

  private :
    std::ifstream          fFile;
    G4String               fPath;

    void                   ReadLineDef        (TString line);
    void                   ReadLineEle        (TString line);
    G4double               ReadVal     (TString str );
    G4String               ReadStr     (TString str );
    G4String               ReadNist    (TString str );

    TPRegexp rgxNum = TPRegexp("\\s*\\d+(\\.\\d*)?([eE][+-]?\\d+)?");
    TPRegexp rgxPar = TPRegexp("\\w+");
    TPRegexp rgxNist= TPRegexp("G4_\\S+");
};



#endif /* end of include guard SLArMATERIALREADER_HH */

