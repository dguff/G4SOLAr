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
#include "G4GDMLParser.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"


SLArAnalysisManagerMsgr::SLArAnalysisManagerMsgr() :
  fMsgrDir  (nullptr), fConstr_(nullptr),
  fCmdDirection(nullptr), fCmdOutputFileName(nullptr),  fCmdOutputPath(nullptr), 
  fCmdGDMLFileName(nullptr), fCmdGDMLExport(nullptr), 
  fGDMLFileName("slar_export.gdml")
{
  TString UIManagerPath = "/SLAr/manager/";
  TString UIGunPath = "/SLAr/gun/";
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
  
  fCmdDirection = 
    new G4UIcmdWith3Vector(UIGunPath+"SetDirection", this);
  fCmdDirection->SetGuidance("Set primary event direction (ν or particle gun)");
  fCmdDirection->SetParameterName("px", "py", "pz", true); 
  fCmdDirection->SetDefaultValue(G4ThreeVector(0, 0, 1));

  fCmdDirectionMode = 
    new G4UIcmdWithAString(UIGunPath+"SetDirectionMode", this);
  fCmdOutputPath->SetGuidance("Set direction mode (fixed, isotropic)");
  fCmdOutputPath->SetParameterName("DirectionMode", true);
  fCmdOutputPath->SetDefaultValue("fixed");

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
}

SLArAnalysisManagerMsgr::~SLArAnalysisManagerMsgr()
{
  G4cerr << "Deleting SLArAnalysisManagerMsgr..." << G4endl;
  if (fMsgrDir          ) delete fMsgrDir          ;
  if (fCmdDirection     ) delete fCmdDirection     ;
  if (fCmdDirectionMode ) delete fCmdDirectionMode ;
  if (fCmdOutputPath    ) delete fCmdOutputPath    ;
  if (fCmdOutputFileName) delete fCmdOutputFileName;
  if (fCmdGDMLFileName  ) delete fCmdGDMLFileName  ;
  if (fCmdGDMLExport    ) delete fCmdGDMLExport    ;
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
  else if (cmd == fCmdDirectionMode) {
    if (G4StrUtil::contains(newVal, "fixed")) {
      SLArAnaMgr->GetEvent()->SetDirectionMode(SLArMCEvent::kFixed); 
    } else if (
        G4StrUtil::contains(newVal, "random") || 
        G4StrUtil::contains(newVal, "isotropic")) 
    {
      SLArAnaMgr->GetEvent()->SetDirectionMode(SLArMCEvent::kRandom);
    } else {
      G4cout << "WARNING: unknown key " << newVal 
        << ". I will assume you want it isotropic" << G4endl; 
      SLArAnaMgr->GetEvent()->SetDirectionMode(SLArMCEvent::kRandom);
    }
  }
  else if (cmd == fCmdDirection) {
    G4ThreeVector dir = G4UIcmdWith3Vector::GetNew3VectorValue(newVal); 
    auto event = SLArAnaMgr->GetEvent(); 
    if (event) event->SetDirection( dir.x(), dir.y(), dir.z() ); 
    else printf("SLArAnalysisManagerMsgr::SetDirection: WARNING event is null!\n"); 
  }
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
}


