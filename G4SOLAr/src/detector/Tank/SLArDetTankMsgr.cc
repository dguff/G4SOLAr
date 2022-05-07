/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArDetTankMsgr
 * @created     : lunedì ago 05, 2019 19:36:48 CEST
 */

#include "SLArDetectorConstruction.hh"
#include "detector/Tank/SLArDetTankMsgr.hh"
#include "detector/Tank/SLArDetTank.hh"

#include "G4RunManager.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "G4UImessenger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4RunManager.hh"
#include "G4PhysicalVolumeStore.hh"

SLArDetTankMsgr::SLArDetTankMsgr(SLArDetectorConstruction* constr_) 
{
  fConstr_ = constr_;
  fDetTank = fConstr_->GetDetTank();

  TString UIDetPath = "/BeamCell/detector/Tank/";

  // directory
  fMsgrDir = new G4UIdirectory(UIDetPath);
  fMsgrDir->SetGuidance("Sub-directory for detector construction via code");

  // create commands
/*
 *  fCmdGap = new G4UIcmdWithABool(UIDetPath+"UseGap", this);
 *  fCmdGap->SetGuidance("Isert Air gap in the tank. Default is false.");
 *  fCmdGap->SetParameterName("UseGap", true);
 *  fCmdGap->SetDefaultValue(false);
 *
 *  fCmdTankRadius = 
 *    new G4UIcmdWithADoubleAndUnit(UIDetPath+"SetTankRadius", this);
 *  fCmdTankRadius->SetGuidance("Set Tank radius");
 *  fCmdTankRadius->SetParameterName("TankRadius", true);
 *  fCmdTankRadius->SetUnitCategory("Length");
 *  fCmdTankRadius->SetDefaultValue(10*cm);
 *  fCmdTankRadius->SetUnitCandidates("mm cm m");
 *      
 *  fCmdBarrelHeight= 
 *    new G4UIcmdWithADoubleAndUnit(UIDetPath+"SetBarrelHeight", this);
 *  fCmdBarrelHeight->SetGuidance("Set Barrel Height");
 *  fCmdBarrelHeight->SetParameterName("BarrelHeight", true);
 *  fCmdBarrelHeight->SetUnitCategory("Length");
 *  fCmdBarrelHeight->SetDefaultValue(15*cm);
 *  fCmdBarrelHeight->SetUnitCandidates("mm cm m");
 *
 *  fCmdEndCapThick= 
 *    new G4UIcmdWithADoubleAndUnit(UIDetPath+"SetEndCapThick", this);
 *  fCmdEndCapThick->SetGuidance("Set Encdcaps thickness");
 *  fCmdEndCapThick->SetParameterName("EndCaptThick", true);
 *  fCmdEndCapThick->SetUnitCategory("Length");
 *  fCmdEndCapThick->SetDefaultValue(5*mm);
 *  fCmdEndCapThick->SetUnitCandidates("mm cm");      
 *
 *  fCmdShellRadius = 
 *    new G4UIcmdWithADoubleAndUnit(UIDetPath+"SetShellRadius", this);
 *  fCmdShellRadius->SetGuidance("Set Air Gap Shell radius");
 *  fCmdShellRadius->SetParameterName("ShellRadius", true);
 *  fCmdShellRadius->SetUnitCategory("Length");
 *  fCmdShellRadius->SetDefaultValue(5*cm);
 *  fCmdShellRadius->SetUnitCandidates("mm cm m");
 *      
 */
}

SLArDetTankMsgr::~SLArDetTankMsgr() 
{
  std::cerr << "Deleting SLArDetTankMsgr..." << std::endl;
  fDetTank = nullptr;
  delete fMsgrDir;
  //delete fCmdGap;
  //delete fCmdTankRadius;
  //delete fCmdShellRadius;
  //delete fCmdEndCapThick;
  //delete fCmdBarrelHeight; 
  std::cerr << "SLArDetTankMsgr DONE" << std::endl;
}

void SLArDetTankMsgr::SetNewValue(G4UIcommand* cmd, G4String newVal) 
{
  G4String parName;
/*
 *  if (cmd == fCmdTankRadius)
 *  {
 *    parName = "BarrelRMin";
 *    G4UIcmdWithADoubleAndUnit* fCmd = (G4UIcmdWithADoubleAndUnit*)cmd;
 *    fDetTank->SetGeoPar(parName, fCmd->GetNewDoubleValue(newVal));
 *
 *    parName = "BarrelRMax";
 *    fDetTank->SetGeoPar(parName, fCmd->GetNewDoubleValue(newVal)+5*mm);
 *
 *    UpdateTank();
 *  }
 *  else if (cmd == fCmdBarrelHeight) 
 *  {
 *    parName = "BarrelHeight";
 *    G4UIcmdWithADoubleAndUnit* fCmd = (G4UIcmdWithADoubleAndUnit*)cmd;
 *    fDetTank->SetGeoPar(parName, fCmd->GetNewDoubleValue(newVal));
 *    UpdateTank();
 *    UpdatePMTs();
 *    UpdateHodo();
 *  }
 *  else if (cmd == fCmdEndCapThick)
 *  {
 *    parName = "ECthickness";
 *    G4UIcmdWithADoubleAndUnit* fCmd = (G4UIcmdWithADoubleAndUnit*)cmd;
 *    fDetTank->SetGeoPar(parName, fCmd->GetNewDoubleValue(newVal));
 *    UpdateTank();
 *    UpdatePMTs();
 *  }
 *  else if (cmd == fCmdGap)
 *  {
 *    G4UIcmdWithABool* fCmd = (G4UIcmdWithABool*)cmd;
 *    fDetTank->SetGap(fCmd->GetNewBoolValue(newVal));
 *    UpdateTank();
 *  }
 *  else if (cmd == fCmdShellRadius)
 *  {
 *    parName = "ShellGapRadius";
 *    G4UIcmdWithADoubleAndUnit* fCmd = (G4UIcmdWithADoubleAndUnit*)cmd;
 *    fDetTank->SetGeoPar(parName, fCmd->GetNewDoubleValue(newVal));
 *    UpdateTank();
 *  }
 */

}

/*
 *void SLArDetTankMsgr::UpdateTank() 
 *{
 *  G4VPhysicalVolume* tankPV = fDetTank->GetModPV();
 *  if ( !tankPV ) return;
 *  else {
 *    G4RotationMatrix* rot = tankPV->GetRotation();
 *    G4ThreeVector     vec = tankPV->GetTranslation();
 *    G4String         name = tankPV->GetName();
 *    G4int          copyNo = tankPV->GetCopyNo();
 *    fDetTank->GetModLV()->RemoveDaughter(tankPV);
 *
 *    delete tankPV; tankPV = nullptr;
 *
 *    fDetTank->ResetTankGeometry();
 *    fDetTank->BuildMaterial();
 *
 *    fDetTank->BuildTank();
 *    fDetTank->GetModPV(name, rot, vec, fConstr_->GetLogicWorld(), 
 *        false, copyNo);
 *    fDetTank->SetVisAttributes();       
 *
 *    G4cout << "SLArDetTankMsgr::UpdateTank" << G4endl;
 *    G4RunManager::GetRunManager()->GeometryHasBeenModified();
 *  }
 *}
 *
 *void SLArDetTankMsgr::UpdatePMTs()
 *{
 *  G4cout << "SLArDetTankMsgr::UpdatePMTs" << G4endl;
 *  G4cout << "PMTs must also be updated" << G4endl;
 *
 *  fConstr_->RemovePMTs();
 *  fConstr_->BuildAndPlacePMTs();
 *
 *  std::map<G4String, SLArDetPMT*> mDetPMT = fConstr_->GetDetPMTs();
 *  for (auto &detPmt : mDetPMT) detPmt.second->SetVisAttributes();
 *
 *  G4RunManager::GetRunManager()->GeometryHasBeenModified();
 *}
 *
 *void SLArDetTankMsgr::UpdateHodo()
 *{
 *  G4cout << "SLArDetTankMsgr::UpdateHodo" << G4endl;
 *  G4cout << "MiniTraker must also be updated" << G4endl;
 *
 *  fConstr_->RemoveHodoModules();
 *  fConstr_->BuildAndPlaceHodoscope();
 *
 *  std::map<G4String, SLArDetHodoscope*> 
 *    mDetHodo = fConstr_->GetDetHodoscopes();
 *  for (auto &detHodo : mDetHodo) detHodo.second->SetVisAttributes();
 *
 *  G4RunManager::GetRunManager()->GeometryHasBeenModified();
 *
 *}
 */


/*
 *void SLArDetTankMsgr::UpdatePMTsMap()
 *{
 *  G4VPhysicalVolume* pmtPV = fConstr_->GetDetPMT()->GetModPV();
 *  if ( !pmtPV ) return;
 *  else 
 *  {
 *    G4RotationMatrix* rot = pmtPV->GetRotation();
 *    G4ThreeVector     vec = pmtPV->GetTranslation();
 *  }
 *
 *}
 */
