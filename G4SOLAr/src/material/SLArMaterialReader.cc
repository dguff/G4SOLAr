/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialReader
 * @created     : mercoledì giu 19, 2019 17:15:17 CEST
 */

#include "material/SLArMaterialReader.hh"

#include "TString.h"
#include "TPRegexp.h"


SLArMaterialReader::SLArMaterialReader() {
  fPath     = "";
  fName     = "";
  fNistName = "";
  fDensity  = 1e-10*g/mole;
  fNComp    = 0;
}

SLArMaterialReader::SLArMaterialReader( G4String name ) {
  SLArMaterialReader();
  fName = name;
}

SLArMaterialReader::SLArMaterialReader( G4String name, G4String path ) {
  SLArMaterialReader();
  fName = name;
  fPath = path;
}

SLArMaterialReader::~SLArMaterialReader() {
  fElVec.clear();
}

void SLArMaterialReader::LoadInfo() {
 
  G4bool isDef     = false;
  G4bool isEle     = false;

  std::ifstream file;
  file.open( fPath.data() );
  
  TString line;
  std::string cline;
  
  while (std::getline(file, cline)) {
    line = cline;
    
    if      (line.BeginsWith("beginDef"))  {isDef = true; continue;}
    else if (line.BeginsWith("endDef"  ))  isDef = false;
    else if (line.BeginsWith("beginEle")) {
      isEle = true;
      fElVec.push_back( SLArElement() );
      continue;
    }
    else if (line.BeginsWith("endEle"))   {
      //G4cout << "Element defined:" << G4endl;
      //fElVec.back().PrintElementSummary();
      //G4cout << "\n\n" << G4endl;
      isEle = false;
    }
      

    if      (isDef) {
      //G4cout <<"\tReading Def: " << line.Data() << "..." << G4endl;
      ReadLineDef(line);
    }
    else if (isEle && !line.BeginsWith("beginEle")) {
      //G4cout <<"\tReading Ele: " << line.Data() << "..." << G4endl;
      ReadLineEle(line);
    }

  }
  file.close();

  PrintMaterialSummary();

}

void SLArMaterialReader::ReadLineDef(TString line) {
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
    fName     = ReadStr(strVal);
  else if (parName == "NIST")
    fNistName = ReadNist(strVal);
  else if (parName == "density")
    fDensity  = ReadVal(strVal) * (g/cm3);
  else if (parName == "ncomp")
    fNComp    = ReadVal(strVal);

}

void SLArMaterialReader::ReadLineEle(TString line) {
  int idx = line.Index(":");
  TString strPar = line;
  strPar.Resize(idx);
  TString strVal;
  std::string sstrVal = line.Data();
  strVal = sstrVal.substr(idx+1);

  TString parName = strPar.operator() (rgxPar);

  if      (parName == "name")
    fElVec.back().fName     = ReadStr(strVal);
  else if (parName == "NIST")
    fElVec.back().fNistName = ReadStr(strVal);
  else if (parName == "symb")
    fElVec.back().fSymb     = ReadStr(strVal);
  else if (parName == "z"   )
    fElVec.back().fZ        = ReadVal(strVal);
  else if (parName == "a"   )
    fElVec.back().fMolMass  = ReadVal(strVal) * (g/mole);
  else if (parName == "nAtom")
    fElVec.back().fNAtom    = ReadVal(strVal);
  else if (parName == "frac")
    fElVec.back().fFraction = ReadVal(strVal);
  else 
    {
      G4cerr << "Unknown element property." << G4endl;
      G4cerr << "Cannot read '"<< line.Data() << "'" << G4endl;
    }
}

G4String SLArMaterialReader::ReadStr(TString str) {
  TString val = str.operator() (rgxPar);
  //G4cout << "ReadStr: " << str << " -> " << val << G4endl;
  return val.Data();
}

G4String SLArMaterialReader::ReadNist(TString str) {
  TString val = str.operator() (rgxNist);
  //G4cout << "ReadNist: " << str << " -> " << val << G4endl;
  return val.Data();
}

G4double SLArMaterialReader::ReadVal(TString str) {
  G4double val = 0;
  TString num = str.operator() (rgxNum);
  val = num.Atof();

  //G4cout << "ReadVal: " << str << " -> " << val << G4endl;
  return val;
}

void SLArMaterialReader::PrintMaterialSummary() {
  G4cout << "* * * * * * * * * * * * * * * * * * * * * * * *" << G4endl;
  G4cout << "* fName     = " << fName     << G4endl;
  G4cout << "* fNistName = " << fNistName << G4endl;
  G4cout << "* fDensity  = " << fDensity/(g/cm3)  << " g/cm3" <<G4endl;
  G4cout << "* fNComp    = " << fNComp    << G4endl;
  G4cout << "* * * * * * * * * * * * * * * * * * * * * * * *" << G4endl;
  G4cout << "\n\n" << G4endl;
  return;

}

void SLArElement::PrintElementSummary() {
  G4cerr << "- -  -  -  -  -  -  -  -  -  -  -  -  -  -  - *" << G4endl;
  G4cerr << "| fName     = " << fName << G4endl;
  G4cerr << "| fNistName = " << fNistName << G4endl;
  G4cerr << "| fSymb     = " << fSymb << G4endl;
  G4cerr << "| fZ        = " << fZ << G4endl;
  G4cerr << "| fNAtom    = " << fNAtom << G4endl; 
  G4cerr << "| fMolMass  = " << fMolMass/(g/mole) << " g/mole" << G4endl; 
  G4cerr << "| fFraction = " << fFraction << G4endl;
  G4cerr << "- -  -  -  -  -  -  -  -  -  -  -  -  -  -  - *" << G4endl;
}
