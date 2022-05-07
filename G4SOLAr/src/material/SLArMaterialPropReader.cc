/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialPropReader
 * @created     : mercoledì giu 19, 2019 17:15:17 CEST
 */

#include "material/SLArMaterialPropReader.hh"

#include "G4MaterialPropertyVector.hh"
#include "G4ConversionUtils.hh"
#include "G4UIcommand.hh"

#include "TString.h"
#include "TPRegexp.h"


SLArMaterialPropReader::SLArMaterialPropReader() {
  fPath     = "";
}

SLArMaterialPropReader::SLArMaterialPropReader( G4String path ) {
  fPath = path;
}

SLArMaterialPropReader::~SLArMaterialPropReader() {
  fPropConst.clear();
  fPropVec  .clear();
}

void SLArMaterialPropReader::ReadFile() {
 
  G4bool isReadingVec     = false;
  G4bool isReadingConst   = false;
  G4bool isReadingSurfDef = false;

  fFile.open( fPath.data() );
  
  TString line;
  std::string cline;
  
  while (std::getline(fFile, cline)) {
    line = cline;
    
    if      (line.BeginsWith("beginPropertyVec"))  {
      isReadingVec = true;
      fPropVec.push_back( SLArMPropVec() );
      continue;
    }
    else if (line.BeginsWith("endPropertyVec"  ))  {
      isReadingVec = false;
      IsReadingData= false;
    }
    else if (line.BeginsWith("beginPropertyConst")) {
      isReadingConst = true;
      fPropConst.push_back( SLArMPropConst() );
      continue;
    }
    else if (line.BeginsWith("endPropertyConst"))    
      isReadingConst = false;
    else if (line.BeginsWith("beginSurfaceDef")) {
      isReadingSurfDef = true;
      continue;
    }
    else if (line.BeginsWith("endSurfaceDef"  )) 
      isReadingSurfDef = false;


    if      (isReadingVec) {
      //G4cout <<"\tReading Property (vector): " << line.Data() << "..." << G4endl;
      ReadLineVec(line);
    }
    else if (isReadingConst) {
      //G4cout <<"\tReading Property (const): " << line.Data() << "..." << G4endl;
      ReadLineConst(line);
    }
    else if (isReadingSurfDef) {
      ReadLineSurfaceDef(line);
    }

  }
  fFile.close();


}

void SLArMaterialPropReader::ReadLineVec(TString line) {
  int idx = line.Index(":");
  TString strPar = line;
  TString strVal = line;

  if ( idx != -1 && !IsReadingData) {
    strPar.Resize(idx);
    strVal = strVal.operator() (idx+1, line.Length());

    TString parName = strPar.operator() (rgxPar);
    //G4cout << "parName = " << strPar << " - strVal = " << strVal << G4endl;

    if      (parName == "name") {
      IsReadingData = false;
      fPropVec.back().fName     = ReadStr(strVal);
    }
    else if (parName == "data")
      IsReadingData = true;
    else 
      IsReadingData = false;
  }
  else if ( idx == -1 && IsReadingData ) {
    idx = line.Index(",");
    TString str1, str2;
    str1 = line.operator() (0, idx);
    str2 = line.operator() (idx+1, line.Length());

    G4double val1, val2;
    
    val1 = ReadVal(str1);
    val2 = ReadVal(str2);

    fPropVec.back().fVec->InsertValues( val1, val2 );
  }
}

void SLArMaterialPropReader::ReadLineConst(TString line) {
  int idx = line.Index(":");
  TString strPar = line;
  TString strVal = line;

  strPar.Resize(idx);
  strVal = strVal.operator() (idx+1, line.Length());

  TString parName = strPar.operator() (rgxPar);

  if      (parName == "name") 
    fPropConst.back().fName = ReadStr(strVal); 
  else if (parName == "value" || parName == "val") {
    G4double val = ReadVal(strVal);
    fPropConst.back().FillValue(val);
  }

}

void SLArMaterialPropReader::ReadLineSurfaceDef(TString line)
{
  int idx = line.Index(":");
  TString strPar = line;
  strPar.Resize(idx);
  TString strVal;
  std::string sstrVal = line.Data();
  strVal = sstrVal.substr(idx+1);

  TString parName = strPar.operator() (rgxPar);
  //G4cout << "parName = " << strPar << 
    //" - strVal = " << strVal << G4endl;

  if      (parName == "name")
    fSurfName     = ReadStr(strVal);
  else if (parName == "model")
  {
    G4String str = ReadStr(strVal);
    if          (str == "glisur" ) fSurfModel = glisur ;
    else if     (str == "unified") fSurfModel = unified;
    else if     (str == "LUT"    ) fSurfModel = LUT;
  }
  else if (parName == "type")
  {
    G4String str = ReadStr(strVal);
    if          (str == "dielectric_dielectric") 
      fSurfType = dielectric_dielectric;
    else if     (str == "dielectric_metal"     ) 
      fSurfType = dielectric_metal;
    else if     (str == "dielectric_LUT"       ) 
      fSurfType = dielectric_LUT;
  }
  else if (parName == "finish")
  {
    G4String str = ReadStr(strVal);
    if          (str == "poslished" ) fSurfFinish = polished;
    else if     (str == "ground"    ) fSurfFinish = ground  ;
    // and many more available, just implement them here.
  }


}


G4String SLArMaterialPropReader::ReadStr(TString str) {
  TString val = str.operator() (rgxPar);
  //G4cout << "ReadStr: " << str << " -> " << val << G4endl;
  return val.Data();
}


G4double SLArMaterialPropReader::ReadVal(TString str) {
  G4double val = 0;
  TString num;
  if       ( str.Contains( rgxUnit ) )
    val = G4UIcommand::ConvertToDimensionedDouble(str.Data());
  else if  ( str.Contains( rgxNum  ) )
    val = G4UIcommand::ConvertToDouble(str.Data());
  else
    G4cout << "SLArMaterialPropReader::ReadVal(): "
           << "No value found in string " << str << G4endl;

  //G4cout << "ReadVal: " << str << " -> " << val << G4endl;
  return val;
}

void SLArMPropConst::FillValue( G4double val ) {
  if      ( fName == "SCINTILLATIONYIELD" )
    fVal = val / MeV;
  else if ( fName == "ISOTHERMAL_COMPRESSIBILITY" )
    fVal = val * m3/MeV;
  else if ( fName == "BIRKS_CONSTANT"     )
    fVal = val * mm/MeV;
  else 
    fVal = val;
}
