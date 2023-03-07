/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArAnalysisManagerMsgr
 * @created     : martedì mar 03, 2020 17:15:44 CET
 */
#include "SLArAnalysisManager.hh"
#include "SLArAnalysisManagerMsgr.hh"

#include "G4RunManager.hh"

#include "G4UImessenger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"

#ifdef SLAR_GDML
#include "G4GDMLParser.hh"
#endif

SLArAnalysisManagerMsgr::SLArAnalysisManagerMsgr() :
  fMsgrDir  (nullptr), fConstr_(nullptr),
  fCmdOutputFileName(nullptr),  fCmdOutputPath(nullptr), 
  fCmdWriteCfgFile(nullptr)
#ifdef SLAR_GDML
  ,fCmdGDMLFileName(nullptr), fCmdGDMLExport(nullptr), 
  fGDMLFileName("slar_export.gdml")
#endif
{
  TString UIManagerPath = "/SLAr/manager/";
  TString UIExportPath = "/SLAr/export/"; 

  fMsgrDir = new G4UIdirectory(UIManagerPath);
  fMsgrDir->SetGuidance("SLAr manager instructions");

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Create commands
  
  fCmdOutputFileName = 
    new G4UIcmdWithAString(UIManagerPath+"SetOutputName", this);
  fCmdOutputFileName->SetGuidance("Set output file name");
  fCmdOutputFileName->SetParameterName("FileName", "slar_output.root");
  fCmdOutputFileName->SetDefaultValue("slar_output.root");

  fCmdOutputPath = 
    new G4UIcmdWithAString(UIManagerPath+"SetOutputFolder", this);
  fCmdOutputPath->SetGuidance("Set output folder");
  fCmdOutputPath->SetParameterName("Path", "./output/");
  fCmdOutputPath->SetDefaultValue("./output/");

  fCmdWriteCfgFile = 
    new G4UIcmdWithAString(UIManagerPath+"WriteCfgFile", this);
  fCmdOutputPath->SetGuidance("Write cfg file to output");
  fCmdOutputPath->SetParameterName("name path", false); 
  
#ifdef SLAR_GDML
  fCmdGDMLFileName = 
    new G4UIcmdWithAString(UIExportPath+"SetGDMLFileName", this); 
  fCmdGDMLFileName->SetGuidance("Set file name for GDML volume export"); 
  fCmdGDMLFileName->SetParameterName("GDMLFileName", true);
  fCmdGDMLFileName->SetDefaultValue(fGDMLFileName.c_str()); 

  fCmdGDMLExport = 
    new G4UIcmdWithAString(UIExportPath+"ExportVolume", this); 
  fCmdGDMLExport->SetGuidance("Export volume to gdml file"); 
  fCmdGDMLExport->SetParameterName("VolumeName", true);
  fCmdGDMLExport->SetDefaultValue("World"); 
#endif
}

SLArAnalysisManagerMsgr::~SLArAnalysisManagerMsgr()
{
  G4cerr << "Deleting SLArAnalysisManagerMsgr..." << G4endl;
  if (fMsgrDir          ) delete fMsgrDir          ;
  if (fCmdOutputPath    ) delete fCmdOutputPath    ;
  if (fCmdOutputFileName) delete fCmdOutputFileName;
  if (fCmdWriteCfgFile  ) delete fCmdWriteCfgFile  ; 
#ifdef SLAR_DGML
  if (fCmdGDMLFileName  ) delete fCmdGDMLFileName  ;
  if (fCmdGDMLExport    ) delete fCmdGDMLExport    ;
#endif

  G4cerr << "SLArAnalysisManagerMsgr DONE" << G4endl;
}

void SLArAnalysisManagerMsgr::SetNewValue
                            (G4UIcommand* cmd, G4String newVal) 
{
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();

  if (cmd == fCmdOutputPath)
    SLArAnaMgr->SetOutputPath(newVal);
  else if (cmd == fCmdOutputFileName)
    SLArAnaMgr->SetOutputName(newVal);
  else if (cmd == fCmdWriteCfgFile) {
    std::stringstream strm;
    strm << newVal.c_str(); 
    std::string name;
    std::string file_path; 
    strm >> name >> file_path; 

    SLArAnaMgr->WriteCfgFile(name, file_path.c_str()); 
  }
#ifdef SLAR_GDML
  else if (cmd == fCmdGDMLFileName) {
    fGDMLFileName = newVal; 
  }
  else if (cmd == fCmdGDMLExport) {
    G4GDMLParser parser; 
    auto pvstore = G4PhysicalVolumeStore::GetInstance(); 
    for (const auto &pv : *pvstore) {
      if (pv->GetName() == newVal) {
        parser.Write(fGDMLFileName, pv); 
      }
    }
  }
#endif
}


