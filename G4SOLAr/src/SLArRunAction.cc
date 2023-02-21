/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArRunAction
 * @created     : venerdì nov 04, 2022 09:28:13 CET
 */

#include "SLArAnalysisManager.hh"
#include "SLArDetectorConstruction.hh"
#include "SLArPrimaryGeneratorAction.hh"
#include "SLArRunAction.hh"
#include "SLArRun.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArRunAction::SLArRunAction()
 : G4UserRunAction(), fG4MacroFile(""), fEventAction(nullptr), fElectronDrift(nullptr)
{ 
  // Create custom SLAr Analysis Manager
  SLArAnalysisManager* anamgr = SLArAnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArRunAction::~SLArRunAction()
{
  delete SLArAnalysisManager::Instance();
  fSDName.clear(); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run* SLArRunAction::GenerateRun() {
  fSDName.push_back("BPolyethilene_1"); 
  fSDName.push_back("BPolyethilene_2"); 
  fSDName.push_back("CryostatWall0");
  fSDName.push_back("CryostatWall1");

  return (new SLArRun(fSDName)); 
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArRunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance(); 
  SLArAnaMgr->CreateFileStructure();

  fElectronDrift = new SLArElectronDrift(); 
  fElectronDrift->ComputeProperties(); 
  fElectronDrift->PrintProperties(); 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArRunAction::EndOfRunAction(const G4Run* aRun)
{

  //- SLArRun object.
  SLArRun* solarRun = (SLArRun*)aRun;
  //--- Dump all socred quantities involved in SLArRun.
  solarRun->DumpAllScorer();
  //---

  auto hmap_capture1 = solarRun->GetHitsMap("BPolyethilene_1", "captureCnts1"); 
  auto hmap_capture2 = solarRun->GetHitsMap("BPolyethilene_2", "captureCnts2"); 
  G4double ncapt_1 = 0.; 
  G4double ncapt_2 = 0.; 
  for (const auto& v : *hmap_capture1) {
    G4double val = *v.second; 
    G4int    idx =  v.first;
    printf("hmap_capture1[%i]: %g\n", idx, val);
    ncapt_1 += val;
  }

  for (const auto& v : *hmap_capture2) {
    G4double val = *v.second; 
    G4int    idx =  v.first;
    printf("hmap_capture2[%i]: %g\n", idx, val);
    ncapt_2 += val;
  }

  printf("%g neutrons capured in layer 1\n", ncapt_1);
  printf("%g neutrons capured in layer 2\n", ncapt_2);


  auto hmap_current_w0 = solarRun->GetHitsMap("CryostatWall0", "nCurrent0"); 
  auto hmap_current_w1 = solarRun->GetHitsMap("CryostatWall1", "nCurrent1"); 
  G4double ncurr_0 = 0; 
  G4double ncurr_1 = 0; 
  for (const auto& v : *hmap_current_w0) {
    G4double val = *v.second; 
    G4int    idx =  v.first;
    printf("hmap_current_w0[%i]: %g\n", idx, val);
    ncurr_0 += val; 
  }

  for (const auto& v : *hmap_current_w1) {
    G4double val = *v.second; 
    G4int    idx =  v.first;
    printf("hmap_current_w1[%i]: %g\n", idx, val);
    ncurr_1 += val; 
  }


  // save histograms & ntuple
  //
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
  SLArAnaMgr->WriteVariable("nEvents", static_cast<G4double>(aRun->GetNumberOfEvent())); 
  SLArAnaMgr->WriteVariable("nCapture_BPolyethilene_1", ncapt_1); 
  SLArAnaMgr->WriteVariable("nCapture_BPolyethilene_2", ncapt_2); 
  SLArAnaMgr->WriteVariable("nCurrent_outerWall", ncurr_0);
  SLArAnaMgr->WriteVariable("nCurrent_innerWall", ncurr_1);

  if (!fG4MacroFile.empty()) {
    SLArAnaMgr->WriteCfgFile("g4macro", fG4MacroFile.c_str()); 
  }

  auto RunMngr = G4RunManager::GetRunManager(); 
  auto SLArDetConstr = 
    (SLArDetectorConstruction*)RunMngr->GetUserDetectorConstruction(); 
  SLArAnaMgr->WriteCfgFile("geometry", SLArDetConstr->GetGeometryCfgFile().c_str());
  SLArAnaMgr->WriteCfgFile("materials", SLArDetConstr->GetMaterialCfgFile().c_str());

  auto SLArGen = (SLArPrimaryGeneratorAction*)RunMngr->GetUserPrimaryGeneratorAction(); 
  if (!SLArGen->GetMarleyConf().empty()) {
    SLArAnaMgr->WriteCfgFile("marley", SLArGen->GetMarleyConf().c_str()); 
  }

  SLArAnaMgr->Save();

  delete fElectronDrift;  fElectronDrift = nullptr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
