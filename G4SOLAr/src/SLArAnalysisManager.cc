/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArAnalysisManager
 * @created     : mercoledì feb 12, 2020 18:26:02 CET
 */

#include "SLArAnalysisManager.hh"
#include <sys/stat.h>

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
    fPDSysCfg(nullptr), fPixSysCfg(nullptr)
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
  fMCEvent->GetReadoutTileSystem()->ConfigSystem(fPixSysCfg);
  SLArEventReadoutTileSystem* evPix = fMCEvent->GetReadoutTileSystem(); 

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


G4bool SLArAnalysisManager::LoadPDSCfg(SLArPDSystemConfig* pdsCfg)
{
  fPDSysCfg = pdsCfg;
  if (!fPDSysCfg) return false;
  else             return true ; 
}

G4bool SLArAnalysisManager::LoadPixCfg(SLArCfgPixSys* pixCfg)
{
  fPixSysCfg = pixCfg;
  if (!fPixSysCfg) return false;
  else             return true ; 
}


void SLArAnalysisManager::WriteSysCfg()
{
  if (!fRootFile)
  {
    G4cout << "SLArAnalysisManager::WriteSysCfg" << G4endl;
    G4cout << "rootfile has null ptr! Quit."   << G4endl;
    return;
  }

  if (fPDSysCfg) {
#ifdef SLAR_DEBUG
    printf("SLArAnalysisManager::WriteSysCfg(): Writing PDSSysConfig... ");
#endif
    fRootFile->cd();
    fPDSysCfg->Write("PDSSysConfig");
#ifdef SLAR_DEBUG
    printf("OK\n");
#endif
  } else {
    G4cout << "SLArAnalysisManager::WritePDSSysConfig" << G4endl;
    G4cout << "fPDSysCfg is nullptr! Quit."      << G4endl;
  }

  if (fPixSysCfg) {
#ifdef SLAR_DEBUG
    printf("SLArAnalysisManager::WriteSysCfg(): Writing PixSysConfig... ");
#endif 
    fRootFile->cd();
    fPixSysCfg->Write("PixSysConfig");
#ifdef SLAR_DEBUG
    printf("OK\n");
#endif
  } else {
    G4cout << "SLArAnalysisManager::WritePixSysConfig" << G4endl;
    G4cout << "fPixSysCfg is nullptr! Quit."      << G4endl;
  }


}

G4bool SLArAnalysisManager::FillEvTree()
{
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
