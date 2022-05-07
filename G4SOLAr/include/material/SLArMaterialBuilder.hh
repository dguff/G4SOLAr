/* *
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialBuilder
 * @created     : mercoledì giu 19, 2019 11:27:57 CEST
 */

#ifndef SLArMATERIALBUILDER_HH

#define SLArMATERIALBUILDER_HH

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"

#include "TSystemDirectory.h"
#include "TList.h"

class SLArMaterialBuilder {
  
  public:
    SLArMaterialBuilder();
    SLArMaterialBuilder(const SLArMaterialBuilder &bldr);
    ~SLArMaterialBuilder();

    void                InitBuilder(G4String matID);
    void                BuildMaterial();
    G4Material*         GetMaterial();
    G4OpticalSurface*   GetSurface();

  private:
    G4String               fMatName              ;
    G4Material*           fMaterial     = nullptr;
    G4MaterialPropertiesTable* fMPT     = nullptr;
    G4OpticalSurface*      fOptSurf     = nullptr;
    G4MaterialPropertiesTable* fSurfMPT = nullptr;
    
    G4bool   fBDir = false; //!< Check for folder 
    TString  fBMatDef = ""; //!< Check for material def
    std::vector<TString>  fBMatPropFiles; //!< Check for MPT file(s)
    std::vector<TString>  fBOptSurfFiles; //!< Check for SPT files
    TSystemDirectory* fMatDir = nullptr ;
    TList* fFileList = nullptr;

    G4String GetNistName();
    void     CheckForDef();
    void     CheckForMPT();
    void     CheckForSPT();
    
    void     BuildMaterialDef();
    void     BuildMaterialMPT();
    void     BuildMaterialOptSurf();

    G4bool   CheckIfRayleigh();
    G4MaterialPropertyVector* 
             CalculateRayleighMeanFreePaths();

};




#endif /* end of include guard SLArMATERIALBUILDER_HH */

