/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArDetTankMsgr
 * @created     : lunedì ago 05, 2019 19:36:18 CEST
 */

#ifndef SLArDETTANKMSGR_HH

#define SLArDETTANKMSGR_HH

class G4UIcommand;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWith3Vector;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class SLArDetectorConstruction;
class SLArDetTank;


#include "G4UImessenger.hh"
#include "globals.hh"

class SLArDetTankMsgr : public G4UImessenger 
{
  
  public:
    explicit SLArDetTankMsgr(SLArDetectorConstruction* constr_);
    ~SLArDetTankMsgr();

  public:
    void SetNewValue(G4UIcommand* cmd, G4String newVal);
    //void UpdateTank();
    //void UpdatePMTs();
    //void UpdateHodo();

  private:
    SLArDetectorConstruction* fConstr_ = nullptr;
    SLArDetTank*              fDetTank = nullptr;
    
    G4UIdirectory*          fMsgrDir = nullptr;

    // define command
    /*
     *G4UIcmdWithABool*                   fCmdGap  = nullptr;
     *G4UIcmdWithADoubleAndUnit*  fCmdTankRadius   = nullptr;
     *G4UIcmdWithADoubleAndUnit*  fCmdBarrelHeight = nullptr;
     *G4UIcmdWithADoubleAndUnit*  fCmdEndCapThick  = nullptr;
     *G4UIcmdWithADoubleAndUnit*  fCmdShellRadius  = nullptr;
     */
};



#endif /* end of include guard SLArDETTANKMSGR_HH */

