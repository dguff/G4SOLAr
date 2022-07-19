/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArAnalysisManager
 * @created     : mercoledì feb 12, 2020 15:03:53 CET
 * @brief Custom TestCell Analysis Manager
 *
 * Custom analysis manager reimplemented from 
 * G4RootAnalysisManager
 */

#ifndef SLArANALYSISMANAGER_HH

#define SLArANALYSISMANAGER_HH

#include "TFile.h"
#include "TTree.h"

#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgSuperCellArray.hh"
#include "event/SLArMCEvent.hh"

#include "SLArAnalysisManagerMsgr.hh"

#include "G4ToolsAnalysisManager.hh"
#include "globals.hh"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSystemConfig; 
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixSystemConfig; 

class SLArAnalysisManager 
{
  public:
    SLArAnalysisManager(G4bool isMaster);
    ~SLArAnalysisManager();

    // static methods
    static SLArAnalysisManager* Instance();
    static G4bool IsInstance();

    G4bool CreateFileStructure();
    G4bool LoadPDSCfg         (SLArPDSystemConfig*  pdsCfg );
    G4bool FillEvTree         ();
    void   SetOutputPath      (G4String path);
    void   SetOutputName      (G4String filename);
    void   WriteSysCfg        ();
    bool   IsPathValid        (G4String path);

    // Access and I/O methods
    TTree* GetTree() const {return  fEventTree;}
    TFile* GetFile() const {return   fRootFile;}
    SLArPDSystemConfig*
           GetPDSCfg()     {return  fPDSysCfg;}
    SLArMCEvent* GetEvent()  {return    fMCEvent;}
    G4bool Save ();

    // mock fake access
    G4bool FakeAccess();

    SLArAnalysisManagerMsgr* fAnaMsgr;

  protected:
    // virtual functions (overriden in MPI implementation)

  private:
    // static data members
    static SLArAnalysisManager*               fgMasterInstance;
    static G4ThreadLocal SLArAnalysisManager* fgInstance;    

    // data members 
    G4bool   fIsMaster;
    G4String fOutputPath;
    G4String fOutputFileName;

    TFile*              fRootFile;
    TTree*              fEventTree;
    SLArMCEvent*          fMCEvent;

    SLArPDSystemConfig * fPDSysCfg;

};

#endif /* end of include guard SLArANALYSISMANAGER_HH */

