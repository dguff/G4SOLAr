/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArRunAction
 * @created     : venerdì nov 04, 2022 09:26:25 CET
 */

#ifndef SLArRunAction_h
#define SLArRunAction_h 1

#include "physics/SLArElectronDrift.hh"

#include "G4UserRunAction.hh"
#include "globals.hh"

class SLArEventAction;

class G4Run;

/// Run action class

class SLArRunAction : public G4UserRunAction
{
  public:
    SLArRunAction();
    virtual ~SLArRunAction();

    virtual G4Run* GenerateRun();
    virtual void   BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
    SLArElectronDrift* GetElectronDrift() {return fElectronDrift;}
    G4String GetG4MacroFile() {return fG4MacroFile;}
    void SetG4MacroFile(const G4String file_path) {fG4MacroFile = file_path;}

  private:
    G4String fG4MacroFile; 
    SLArEventAction* fEventAction;
    SLArElectronDrift* fElectronDrift; 

    std::vector<G4String> fSDName;  
};


#endif
