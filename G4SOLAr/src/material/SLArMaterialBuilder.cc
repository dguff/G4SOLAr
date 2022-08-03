/*  *  *  *  *  *  *  *  *  *  *  *  * 
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialBuilder
 * @created     : mercoledì giu 19, 2019 11:38:26 CEST
\*  *  *  *  *  *  *  *  *  *  *  *  */

#include <map>

#include "SLArUserPath.hh"
#include "material/SLArMaterialBuilder.hh"
#include "material/SLArMaterialReader.hh"
#include "material/SLArMaterialPropReader.hh"

#include "SLArUserPath.hh"

#include "G4Element.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4OpRayleigh.hh"
#include "G4PhysicalConstants.hh"
#include "TSystemDirectory.h"
#include "TIterator.h"

#include "rapidjson/document.h"

SLArMaterialBuilder::SLArMaterialBuilder()  {}

SLArMaterialBuilder::SLArMaterialBuilder(const SLArMaterialBuilder &bldr)
{
  fMatName       = bldr.fMatName      ;
  fMaterial      = bldr.fMaterial     ;
  fMPT           = bldr.fMPT          ;
  fOptSurf       = bldr.fOptSurf      ;
  fSurfMPT       = bldr.fSurfMPT      ;
  fBDir          = bldr.fBDir         ;
  fBMatDef       = bldr.fBMatDef      ;
  fBMatPropFiles = bldr.fBMatPropFiles;
  fBOptSurfFiles = bldr.fBOptSurfFiles;
  fMatDir        = bldr.fMatDir       ;
  fFileList      = bldr.fFileList     ;
}

SLArMaterialBuilder::~SLArMaterialBuilder() {}

void SLArMaterialBuilder::InitBuilder(G4String matID) {

  fMatName = matID;
  fMatDir  = new TSystemDirectory("dir", 
        Form("%s/%s", SLAR_MATERIAL_DIR, matID.data()));

  fFileList = fMatDir->GetListOfFiles();

  if ( !fFileList ) {
    fBDir = false;
    G4cout << matID.data() << 
      " folder not found. Trying NIST database" << G4endl;
  }
  else {
    fBDir = true;
    G4cout << matID.data() << 
      " folder found. " << G4endl;
    CheckForDef();
    CheckForMPT();
    CheckForSPT();
  }
}



void SLArMaterialBuilder::BuildMaterialDef() {
  G4NistManager* nistManager = G4NistManager::Instance();
  if ( !fBDir ) 
  {
    G4String nistName = GetNistName();
    fMaterial = nistManager->FindOrBuildMaterial(nistName.data());
  }
  else {
    if ( fBMatDef.IsNull() )
    {
      G4cout << "No def file found. Try NIST database" << G4endl;
      G4String nistName = GetNistName();
      fMaterial = nistManager->FindOrBuildMaterial(nistName.data());
    }
    else 
    {
      G4cout << "def file found. Reading file..." << G4endl;
      SLArMaterialReader reader( fMatName,  
          Form("%s/%s", fMatDir->GetTitle(), fBMatDef.Data() ) );
      reader.LoadInfo();

      if (reader.fNistName != "") {
        nistManager->FindOrBuildMaterial(reader.fNistName); 
        fMaterial = G4Material::GetMaterial( reader.fNistName );
        G4cerr<< "SLArMaterialBuilder::BuildMaterialDef(): " << 
          reader.fNistName << G4endl;
      }
      else {
        G4cerr<< "SLArMaterialBuilder::BuildMaterialDef(): " << 
          fMatName << G4endl;
        fMaterial = new G4Material(
            reader.fName,
            reader.fDensity,
            reader.fNComp
            );
        for (int n=0; n<reader.fNComp; n++)
        {
          if (reader.fElVec.at(n).fNistName != "") 
          {
            G4cerr << "Adding " << 
              reader.fElVec.at(n).fNistName << " to " << 
              fMaterial->GetName();
            fMaterial->AddElement(
               nistManager->FindOrBuildElement(
                 reader.fElVec.at(n).fNistName), 
               reader.fElVec.at(n).fFraction);
          }
          else 
          {
            G4cerr << "Adding " << 
              reader.fElVec.at(n).fNistName << " to " << 
              fMaterial->GetName();
            fMaterial->AddElement(
                new G4Element(
                  reader.fElVec.at(n).fName,
                  reader.fElVec.at(n).fSymb,
                  reader.fElVec.at(n).fZ,
                  reader.fElVec.at(n).fMolMass
                  ), 
                reader.fElVec.at(n).fNAtom);
          }
        }
      }
    }
  }
}

void SLArMaterialBuilder::BuildMaterialMPT() {
  if ( fBMatPropFiles.size() > 0 ) {
    fMPT = new G4MaterialPropertiesTable();

    for ( auto &file : fBMatPropFiles ) {
      SLArMaterialPropReader reader( 
          Form("%s/%s", fMatDir->GetTitle(), file.Data())); 
      reader.ReadFile();

      for (auto &p : reader.fPropVec) {
        G4cout << "SLArMaterialBuilder::BuildMaterialMPT(): "
               << "Adding " << p.fName << " to material table" << G4endl;
        fMPT->AddProperty( p.fName, p.fVec );
      }
      for (auto &p : reader.fPropConst)
        if ( !p.fName.contains("BIRKS") ) {
          G4cout << "SLArMaterialBuilder::BuildMaterialMPT(): "
                 << "Adding " << p.fName << " to material table" << G4endl;
          fMPT->AddConstProperty( p.fName, p.fVal );
        }
        else if ( p.fName == "BIRKS_CONSTANT" )
          fMaterial->GetIonisation()->SetBirksConstant( p.fVal );
        else
          G4cout << "SLArMaterialBuilder::BuildMaterialMPT(): " 
                 << "Uniknown field " << p.fName << G4endl;
    }
    
  }

}

void SLArMaterialBuilder::BuildMaterialOptSurf()
{
  if ( fBOptSurfFiles.size() > 0 ) 
  {
    fSurfMPT = new G4MaterialPropertiesTable();
    fOptSurf = new G4OpticalSurface("empty_surf");

    for ( auto &file : fBOptSurfFiles ) {
      SLArMaterialPropReader reader( 
          Form("%s/%s", fMatDir->GetTitle(), file.Data())); 
      reader.ReadFile();

      fOptSurf->SetName  (reader.fSurfName  );
      fOptSurf->SetModel (reader.fSurfModel );
      fOptSurf->SetType  (reader.fSurfType  );
      fOptSurf->SetFinish(reader.fSurfFinish);

      for (auto &p : reader.fPropVec) {
        fSurfMPT->AddProperty( p.fName, p.fVec );
      }

      for (auto &p : reader.fPropConst) {
        if ( !p.fName.contains("BIRKS") ) {
          fSurfMPT->AddConstProperty( p.fName, p.fVal );
        }
        else if ( p.fName == "BIRKS_CONSTANT" )
          fMaterial->GetIonisation()->SetBirksConstant( p.fVal );
        else
          G4cout << "SLArMaterialBuilder::BuildMaterialOptSurfMPT(): " 
            << "Unknown field " << p.fName << G4endl;
      }
    }
  }
  else 
  {
    G4cout << "SLArMaterialBuilder::BuildMaterialOptSurf(): " 
      << "No surface properties found" << G4endl;
  }

}


void SLArMaterialBuilder::BuildMaterial() {
  BuildMaterialDef();

  if ( fBMatPropFiles.size() > 0 ) {
    BuildMaterialMPT();
    //fMPT->DumpTable();
    fMaterial->SetMaterialPropertiesTable( fMPT );   
    
    // Build RAYLEIGH SCATTERING table (in case not already done)
    if (CheckIfRayleigh()) {
      G4MaterialPropertyVector* vecRayleigh = 
        CalculateRayleighMeanFreePaths();
      if ( vecRayleigh ) 
        fMPT->AddProperty("RAYLEIGH", vecRayleigh);
      else 
        G4cout << "SLArMaterialBuilder::BuildMaterialMPT(): "
          << "Impossible to build RAYLEIGH" << G4endl; 
    }

  }

  if ( fBOptSurfFiles.size() > 0 ) 
  {
    BuildMaterialOptSurf();
    fOptSurf->SetMaterialPropertiesTable(fSurfMPT); 
  }
}

G4Material* SLArMaterialBuilder::GetMaterial() {
  return fMaterial;
}

G4OpticalSurface* SLArMaterialBuilder::GetSurface() {
  return fOptSurf;
}

G4String SLArMaterialBuilder::GetNistName() {
  TString nistName = fMatName;
  if ( nistName.Length() > 2 ) 
  {
    nistName.ToUpper();
    nistName = "G4_"+nistName; 
  }
  else nistName = "G4_"+nistName;

  return nistName.Data();

}

void SLArMaterialBuilder::CheckForDef() {
  TString filename;
  for (int i=0; i<fFileList->GetEntries(); i++) {
    filename = fFileList->At(i)->GetName();
    TString extension = filename.operator() 
      (filename.Index(".")+1, filename.Length());
    if (extension=="def") {
      fBMatDef = fFileList->At(i)->GetName();
      return;
    }
  }
  G4cerr << "SLArMaterialReader::CheckForDef() " <<
    fMatName << " def file not found" << G4endl;
}

void SLArMaterialBuilder::CheckForMPT() {
  TString filename;
  for (int i=0; i<fFileList->GetEntries(); i++) {
    filename = fFileList->At(i)->GetName();
    TString extension = filename.operator() 
      (filename.Index(".")+1, filename.Length());
    if (extension=="mpt") {
      fBMatPropFiles.push_back(fFileList->At(i)->GetName());
      G4cout << fBMatPropFiles.back() << " file found." << G4endl;
    }
  }
}

void SLArMaterialBuilder::CheckForSPT() {
  TString filename;
  for (int i=0; i<fFileList->GetEntries(); i++) {
    filename = fFileList->At(i)->GetName();
    TString extension = filename.operator() 
      (filename.Index(".")+1, filename.Length());
    if (extension=="spt") {
      fBOptSurfFiles.push_back(fFileList->At(i)->GetName());
      G4cout << fBOptSurfFiles.back() << " file found." << G4endl;
    }
  }
}

G4bool SLArMaterialBuilder::CheckIfRayleigh() {
  G4bool answer = false;

  G4MaterialPropertyVector* vec = nullptr;
  vec = fMaterial->GetMaterialPropertiesTable()->GetProperty("RAYLEIGH");

  if ( vec ) { answer = false; return answer; }
  else {
    if ( fMaterial->GetMaterialPropertiesTable()->
          ConstPropertyExists("ISOTHERMAL_COMPRESSIBILITY") )
      {answer = true; return answer;}
    else {
      G4cout << "SLArMaterialBuilder::CheckIfRayleigh(): "
             << fMaterial->GetName() << " has no isotherm. compressibility."
             << " Cannot compute Rayleigh att. lenght." << G4endl;
      answer = false;
    }
  }

  return answer;
}

G4MaterialPropertyVector* 
SLArMaterialBuilder::CalculateRayleighMeanFreePaths() {
  G4MaterialPropertiesTable* materialProperties = 
    fMaterial->GetMaterialPropertiesTable();

  // Retrieve the beta_T or isothermal compressibility value. For backwards
  // compatibility use a constant if the material is "Water". If the material
  // doesn't have an ISOTHERMAL_COMPRESSIBILITY constant then return
  G4double betat;
  if ( fMaterial->GetName() == "Water" )
    betat = 7.658e-23*CLHEP::m3/CLHEP::MeV;
  else if(materialProperties->ConstPropertyExists("ISOTHERMAL_COMPRESSIBILITY"))
    betat = materialProperties->GetConstProperty(kISOTHERMAL_COMPRESSIBILITY);
  else
    return NULL;

  // If the material doesn't have a RINDEX property vector then return
  G4MaterialPropertyVector* rIndex = materialProperties->GetProperty(kRINDEX);
  if ( rIndex == NULL ) return NULL;

  // Retrieve the optional scale factor, (this just scales the scattering length
  G4double scaleFactor = 1.0;
  if( materialProperties->ConstPropertyExists( "RS_SCALE_FACTOR" ) )
    scaleFactor= materialProperties->GetConstProperty(kRS_SCALE_FACTOR );

  // Retrieve the material temperature. For backwards compatibility use a 
  // constant if the material is "Water"
  G4double temperature;
  if( fMaterial->GetName() == "Water" )
    temperature = 283.15*CLHEP::kelvin; // Temperature of water is 10 degrees celsius
  else
    temperature = fMaterial->GetTemperature();

  G4PhysicsFreeVector* rayleighMeanFreePaths =
    new G4PhysicsFreeVector();
  // This calculates the meanFreePath via the Einstein-Smoluchowski formula
  const G4double c1 = scaleFactor * betat * temperature * k_Boltzmann / 
    ( 6.0 * pi );

  for( size_t uRIndex = 0; uRIndex < rIndex->GetVectorLength(); uRIndex++ )
  {
    const G4double energy = rIndex->Energy( uRIndex );
    const G4double rIndexSquared = (*rIndex)[uRIndex] * (*rIndex)[uRIndex];
    const G4double xlambda = h_Planck * c_light / energy;
    const G4double c2 = std::pow(twopi/xlambda,4);
    const G4double c3 = 
      std::pow(((rIndexSquared-1.0)*(rIndexSquared+2.0 )/3.0),2);

    const G4double meanFreePath = 1.0 / ( c1 * c2 * c3 );


    rayleighMeanFreePaths->InsertValues( energy, meanFreePath );
  }

  return rayleighMeanFreePaths;
}
