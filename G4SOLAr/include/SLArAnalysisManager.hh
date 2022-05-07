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

//#include "config/SLArSystemConfigHodo.hh"
//#include "config/SLArSystemConfigPMT.hh"
#include "event/SLArMCEvent.hh"

#include "SLArAnalysisManagerMsgr.hh"

#include "G4ToolsAnalysisManager.hh"
#include "globals.hh"

class SLArAnalysisManager 
{
  public:
    SLArAnalysisManager(G4bool isMaster);
    ~SLArAnalysisManager();

    // static methods
    static SLArAnalysisManager* Instance();
    static G4bool IsInstance();

    G4bool CreateFileStructure();
    //G4bool LoadPMTCfg         (SLArSystemConfigPMT*  pmtCfg );
    //G4bool LoadHodoCfg        (SLArSystemConfigHodo* hodoCfg);
    G4bool FillEvTree         ();
    void   SetOutputPath      (G4String path);
    void   SetOutputName      (G4String filename);
    void   WriteSysCfg        ();
    bool   IsPathValid        (G4String path);

    // Access methods
    TTree* GetTree() const {return  fEventTree;}
    TFile* GetFile() const {return   fRootFile;}
    //SLArSystemConfigPMT*
           //GetPMTCfg()     {return  fPMTSysCfg;}
    //SLArSystemConfigHodo*
           //GetHodoCfg()    {return fHodoSysCfg;}
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

    // methods
    //void ClearNtupleManagers();
    //void CreateNtupleManagers();
    //G4int  GetNtupleFileNumber();
    //G4bool ResetNtuple();

    //G4bool WriteH1();
    //G4bool WriteH2();
    //G4bool WriteH3();
    //G4bool WriteP1();
    //G4bool WriteP2();

    // data members 
    G4bool   fIsMaster;
    G4String fOutputPath;
    G4String fOutputFileName;

    TFile*              fRootFile;
    TTree*              fEventTree;
    SLArMCEvent*          fMCEvent;

    //SLArSystemConfigPMT * fPMTSysCfg;
    //SLArSystemConfigHodo* fHodoSysCfg;

};

#endif /* end of include guard SLArANALYSISMANAGER_HH */

