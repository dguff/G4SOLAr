/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArAnalysisManager
 * @created     : mercoledì feb 12, 2020 18:26:02 CET
 */

#include "SLArAnalysisManager.hh"
#include <sys/stat.h>
#include <fstream>
#include <sstream>

#include "TObjString.h"
#include "TVectorD.h"
#include "TParameter.h"

SLArAnalysisManager* SLArAnalysisManager::fgMasterInstance = nullptr;
G4ThreadLocal SLArAnalysisManager* SLArAnalysisManager::fgInstance = nullptr;

//__________________________________________________________________
SLArAnalysisManager* SLArAnalysisManager::Instance()
{
  if ( fgInstance == nullptr ) {
    G4bool isMaster = ! G4Threading::IsWorkerThread();
    fgInstance = new SLArAnalysisManager(isMaster);
  }
  
  return fgInstance;
}    

//__________________________________________________________________
G4bool SLArAnalysisManager::IsInstance()
{
  return ( fgInstance != 0 );
}    

SLArAnalysisManager::SLArAnalysisManager(G4bool isMaster)
  : fAnaMsgr  (nullptr),
    fIsMaster(isMaster), fOutputPath(""),
    fOutputFileName("solarsim_output.root"), 
    fRootFile (nullptr), fEventTree (nullptr), 
    fMCEvent  (nullptr), 
    fPDSysCfg(nullptr)
{
  if ( ( isMaster && fgMasterInstance ) || ( fgInstance ) ) {
    G4ExceptionDescription description;
    description 
      << "      " 
      << "SLArAnalysisManager already exists." 
      << "Cannot create another instance.";
    G4Exception("SLArAnalysisManager::SLArAnalysisManager()",
        "Analysis_F001", FatalException, description);
  }
  if ( isMaster ) {
    fgMasterInstance = this;
    fMCEvent         = new SLArMCEvent();
    fAnaMsgr         = new SLArAnalysisManagerMsgr();
  }
  fgInstance = this;
}

//______________________________________________________________
SLArAnalysisManager::~SLArAnalysisManager()
{
  G4cerr << "Deleting SLArAnalysisManager" << G4endl;
  if (fRootFile        ) {
    if (fRootFile->IsOpen()) {
      fRootFile->cd();
      fEventTree->Write();
      fEventTree->Delete();
      fRootFile->Close(); 
    }
  }
  if ( this->fIsMaster ) fgMasterInstance = nullptr;
  if ( fAnaMsgr        ) delete  fAnaMsgr  ; 
  fgInstance = nullptr;
  G4cerr << "SLArAnalysisManager DONE" << G4endl;
}

G4bool SLArAnalysisManager::CreateFileStructure()
{
  G4String filepath = fOutputPath;
  filepath.append(fOutputFileName);
  fRootFile = new TFile(filepath, "recreate");

  if (!fRootFile)
  {
    G4cout << "SLArAnalysisManager::CreateFileStructure\n" << G4endl;
    G4cout << "rootfile not created! Quit."              << G4endl;
    return false;
  }

  fEventTree = new TTree("EventTree", "Event Tree");
  
  fMCEvent->ConfigAnode( fAnodeCfg ); 

  //if (fPDSysCfg) fMCEvent->GetSuperCellSystem()->ConfigSystem(fPDSysCfg);
  if (fPDSysCfg) fMCEvent->ConfigSuperCellSystem(fPDSysCfg);

  fEventTree->Branch("MCEvent", &fMCEvent);

  return true;
}

G4bool SLArAnalysisManager::Save()
{
  if (!fRootFile) return false;

  if (fEventTree) {
    fEventTree->Write();
    fEventTree->Delete();
  }

  WriteSysCfg(); 

  fRootFile->Close();

  return true;
}


G4bool SLArAnalysisManager::LoadPDSCfg(SLArCfgSystemSuperCell* pdsCfg)
{
  fPDSysCfg = pdsCfg;
  if (!fPDSysCfg) return false;
  else             return true ; 
}

G4bool SLArAnalysisManager::LoadAnodeCfg(SLArCfgAnode* anodeCfg)
{
  if (fAnodeCfg.count(anodeCfg->GetTPCID())) {
    printf("SLArAnalysisManager::LoadAnodeCfg WARNING "); 
    printf("an anode configuration with index %i is already registered. skip.\n",
        anodeCfg->GetIdx());
    return false;
  }

  fAnodeCfg.insert(std::make_pair(anodeCfg->GetTPCID(), anodeCfg));

  return true;
}

void SLArAnalysisManager::WriteSysCfg() {
  if (!fRootFile) {
    G4cout << "SLArAnalysisManager::WriteSysCfg" << G4endl;
    G4cout << "rootfile has null ptr! Quit."   << G4endl;
    return;
  }

  if (fPDSysCfg) {
    fRootFile->cd();
    fPDSysCfg->Write("PDSSysConfig");
  } else {
    G4cout << "SLArAnalysisManager::WritePDSSysConfig" << G4endl;
    G4cout << "fPDSysCfg is nullptr! Quit."      << G4endl;
  }

  for (auto &anodeCfg : fAnodeCfg) {
    fRootFile->cd();
    anodeCfg.second->Write(Form("AnodeCfg%i", anodeCfg.second->GetIdx()));
  } 
  return;
}

G4bool SLArAnalysisManager::FillEvTree() {
#ifdef SLAR_DEBUG
  printf("SLArAnalysisManager::FillEvTree...");
#endif
  if (!fEventTree) {
#ifdef SLAR_DEBUG
    printf(" EventTree is NULL!\n");
#endif
    return false;
  }
  
  fEventTree->Fill();
#ifdef SLAR_DEBUG
  printf(" OK\n");
#endif
  return true;
}

int SLArAnalysisManager::WriteVariable(G4String name, G4double val) {
  if (!fRootFile) {
    printf("SLArAnalysisManager::WriteVariable WARNING ");
    printf("rootfile not present yet. Cannot write %s variable.\n", 
        name.c_str());
    return 666;
  } 

  TParameter<double> var(name, val); 
  fRootFile->cd(); 
  int status = var.Write(); 
  return status; 
}

int SLArAnalysisManager::WriteArray(G4String name, G4int size, G4double* val) {
  if (!fRootFile) {
    printf("SLArAnalysisManager::WriteVariable WARNING ");
    printf("rootfile not present yet. Cannot write %s variable.\n", 
        name.c_str());
    return 666;
  } 

  TVectorD var(size, val); 
  fRootFile->cd(); 
  int status = var.Write(name); 
  return status; 
}

int SLArAnalysisManager::WriteCfg(G4String name, const char* cfg) {
  if (!fRootFile) {
    printf("SLArAnalysisManager::WriteVariable WARNING ");
    printf("rootfile not present yet. Cannot write %s variable.\n", 
        name.c_str());
    return 666;
  } 

  TObjString cfg_str(cfg); 
  fRootFile->cd(); 
  int status = cfg_str.Write(name); 
  return status; 
}

int SLArAnalysisManager::WriteCfgFile(G4String name, const char* path) {
  std::ifstream ifile; 
  ifile.open(path); 
  if (!ifile.is_open()) {
    printf("SLArAnalysisManager::WriteCfgFile WARNING ");
    printf("Unable to open file %s\n", path);
    return 4; 
  }

  std::stringstream strm; 
  strm << ifile.rdbuf(); 
  const std::string buff(strm.str()); 

  WriteCfg(name, buff.c_str()); 

  ifile.close(); 

  return 0;
}

G4bool SLArAnalysisManager::FakeAccess()
{
  G4cout << "SLArAnalysisManager::FakeAccess() - still alive" 
         << G4endl;

  return true;
}

bool SLArAnalysisManager::IsPathValid(G4String path)
{
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

void SLArAnalysisManager::SetOutputPath(G4String path)
{
  G4String spath = path;
  if ( IsPathValid(spath) ) 
  {
    // check is path is file 
    struct stat s;
    stat(path, &s);
    if (s.st_mode & S_IFDIR)
    { // append '/' if necessary 
      G4String last = &spath.operator[](spath.length()-1);
      if ( G4StrUtil::icompare(last, "/") ) spath.append("/");
      fOutputPath = spath;
      return;
    }
    else 
    {
      std::cerr << "Output file path is not a directory! "
                << "(maybe an existing file?)"
                << std::endl;
      fOutputPath = "./";
      return;
    }
  }
  else 
  {
    // Creating a new directory 
    if (mkdir(path, 0777) == -1) 
      std::cerr << "Error :  " << strerror(errno) << std::endl; 

    else
    {
      G4String last = &spath.operator[](spath.length()-1);
      if ( G4StrUtil::icompare(last, "/") ) spath.append("/");
      fOutputPath = spath;
      std::cout << "Directory created" << std::endl; 
    }
  }
}

void SLArAnalysisManager::SetOutputName(G4String filename)
{
  if (! G4StrUtil::contains(filename, ".root") ) filename.append(".root");
  fOutputFileName = filename;
}

void SLArAnalysisManager::RegisterPhyicsBiasing(G4String particle_name, G4double biasing_factor) {
  if (fBiasing.count(particle_name) > 0) {
    printf("SLArAnalysisManager::RegisterPhyicsBiasing WARNING\n");
    printf("%s is already biased, updating biasing factor\n", particle_name.data());
    fBiasing[particle_name] = biasing_factor;
    return;
  }
  else {
    fBiasing.insert( std::make_pair(particle_name, biasing_factor) ); 
    return;
  }
}
