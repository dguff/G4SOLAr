/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArPrimaryGeneratorMessenger.cc
 * @created     Friday Dec 30, 2022 18:48:41 CET
 */


#include "SLArPrimaryGeneratorMessenger.hh"

#include "SLArPrimaryGeneratorAction.hh"
#include "SLArBulkVertexGenerator.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorMessenger::
  SLArPrimaryGeneratorMessenger(SLArPrimaryGeneratorAction* SLArGun)
  : G4UImessenger(),
    fSLArAction(SLArGun)
{
  fCmdGunDir = new G4UIdirectory("/SLAr/gen/");
  fCmdGunDir->SetGuidance("PrimaryGenerator control");

  fCmdGenerator= 
    new G4UIcmdWithAString("/SLAr/gen/type", this);
  fCmdGenerator->SetGuidance("Set SOLAr generator");
  fCmdGenerator->SetGuidance("(ParticleGun, Decay0, ...)");
  fCmdGenerator->SetParameterName("Mode", false);
  fCmdGenerator->SetDefaultValue("ParticleGun");
  fCmdGenerator->SetCandidates("ParticleGun Decay0 Marley ExternalGen Genie");

  fCmdParticle= 
    new G4UIcmdWithAString("/SLAr/gen/particle", this);
  fCmdParticle->SetGuidance("Set particle gun particle");
  fCmdParticle->SetParameterName("particle_name", false);
  fCmdParticle->SetDefaultValue("electron");

  fCmdBulkVol= 
    new G4UIcmdWithAString("/SLAr/gen/volume", this); 
  fCmdBulkVol->SetGuidance("Set bulk volume for bulk event generation"); 
  fCmdBulkVol->SetGuidance("(Physical Volume name)"); 
  fCmdBulkVol->SetParameterName("PhysVol", true, false); 
  fCmdBulkVol->SetDefaultValue("target_lar_pv"); 

  fCmdBulkVolFraction= 
    new G4UIcmdWithADouble("/SLAr/gen/volumeFraction", this); 
  fCmdBulkVol->SetGuidance("Set fraction of the bulk volume for event generation"); 
  fCmdBulkVol->SetGuidance("<Volume Fraction>"); 
  fCmdBulkVol->SetParameterName("volFrac", true, false); 
  fCmdBulkVol->SetDefaultValue("1.0"); 

  fCmdMarley= 
    new G4UIcmdWithAString("/SLAr/gen/marleyconf", this); 
  fCmdMarley->SetGuidance("Set MARLEY configuration file"); 
  fCmdMarley->SetGuidance("(configuration file path)"); 
  fCmdMarley->SetParameterName("marley_config", true, false); 
  fCmdMarley->SetDefaultValue("marley_default.json"); 

  fCmdBackgoundConf= 
    new G4UIcmdWithAString("/SLAr/gen/backgroundconf", this); 
  fCmdBackgoundConf->SetGuidance("Set backgound configuration file"); 
  fCmdBackgoundConf->SetGuidance("(configuration file path)"); 
  fCmdBackgoundConf->SetParameterName("background_config", true, false); 

  fCmdExternalConf= 
    new G4UIcmdWithAString("/SLAr/gen/externalconf", this); 
  fCmdExternalConf->SetGuidance("Set external backgound configuration file"); 
  fCmdExternalConf->SetGuidance("(configuration file path)"); 
  fCmdExternalConf->SetParameterName("external_background_config", true, false);
  
  fCmdGenieInput= 
    new G4UIcmdWithAString("/SLAr/gen/genieinput", this); 
  fCmdGenieInput->SetGuidance("Set GENIE input file"); 
  fCmdGenieInput->SetGuidance("(input file path)"); 
  fCmdGenieInput->SetParameterName("genie_input", true, false);


  fCmdDirectionMode = 
    new G4UIcmdWithAString("/SLAr/gen/SetDirectionMode", this);
  fCmdDirectionMode->SetGuidance("Set direction mode (fixed, isotropic)");
  fCmdDirectionMode->SetParameterName("DirectionMode", true);
  fCmdDirectionMode->SetDefaultValue("fixed");

  fCmdEnergy = 
    new G4UIcmdWithADoubleAndUnit("/SLAr/gen/energy", this);
  fCmdEnergy->SetGuidance("Set particle kinetic energy");
  fCmdEnergy->SetParameterName("Energy", false);
  fCmdEnergy->SetDefaultValue(1*CLHEP::MeV);

  fCmdGunPosition = 
    new G4UIcmdWith3VectorAndUnit("/SLAr/gen/gunPosition", this);
  fCmdGunPosition->SetGuidance("Set position of the generated events");
  fCmdGunPosition->SetParameterName("posX", "posY", "posZ", false);
  fCmdGunPosition->SetDefaultValue(G4ThreeVector(0*CLHEP::cm, 0*CLHEP::cm, 0*CLHEP::cm));

  fCmdGunDirection = 
    new G4UIcmdWith3Vector("/SLAr/gen/direction", this);
  fCmdGunDirection->SetGuidance("Set event momentum direction");
  fCmdGunDirection->SetParameterName("p_x", "p_y", "p_z", false); 
  fCmdGunDirection->SetDefaultValue( G4ThreeVector(0, 0, 1)); 

  fCmdTracePhotons = 
    new G4UIcmdWithABool("/SLAr/phys/DoTracePhotons", this); 
  fCmdTracePhotons->SetGuidance("Set/unset tracing of optical photons"); 
  fCmdTracePhotons->SetParameterName("do_trace", false, true); 
  fCmdTracePhotons->SetDefaultValue(true);

  fCmdDriftElectrons = 
    new G4UIcmdWithABool("/SLAr/phys/DoDriftElectrons", this); 
  fCmdDriftElectrons->SetGuidance("Set/unset drift and collection of ionization electrons"); 
  fCmdDriftElectrons->SetParameterName("do_trace", false, true); 
  fCmdDriftElectrons->SetDefaultValue(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArPrimaryGeneratorMessenger::~SLArPrimaryGeneratorMessenger()
{
  delete fCmdGenerator;
  delete fCmdParticle;
  delete fCmdBulkVol;
  delete fCmdEnergy;
  delete fCmdBulkVolFraction;
  delete fCmdMarley;
  delete fCmdBackgoundConf;
  delete fCmdGenieInput;
  delete fCmdGunPosition;
  delete fCmdGunDirection;
  delete fCmdGunDir;
  delete fCmdTracePhotons; 
  delete fCmdDriftElectrons;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArPrimaryGeneratorMessenger::SetNewValue(
              G4UIcommand* command, G4String newValue)
{

  if (command == fCmdGenerator) 
  {
    EGenerator gen = kParticleGun;
    G4String strMode = newValue;
    if      (G4StrUtil::contains(strMode, "Gun"   )) gen = kParticleGun;
    else if (G4StrUtil::contains(strMode, "Decay0")) gen = kDecay0;
    else if (G4StrUtil::contains(strMode, "Marley")) gen = kMarley;
    else if (G4StrUtil::contains(strMode, "ExternalGen")) gen = kExternalGen;
    else if (G4StrUtil::contains(strMode, "Genie")) gen = kGenie;

    fSLArAction->SetGenerator(gen);
  } 
  else if (command == fCmdBulkVol) { 
    G4String vol = newValue; 
    fSLArAction->SetBulkName(vol); 
  }
  else if (command == fCmdParticle) {
    G4String particle = newValue; 
    fSLArAction->SetPGunParticle(particle);
  }
  else if (command == fCmdBulkVolFraction) {
    G4double frac = fCmdBulkVolFraction->GetNewDoubleValue(newValue); 
    fSLArAction->fBulkGenerator->SetFiducialFraction(frac);
  }
  else if (command == fCmdMarley) {
    fSLArAction->SetMarleyConf(newValue); 
  } 
  else if (command == fCmdBackgoundConf) {
    fSLArAction->SetBackgroundConf(newValue); 
  }
  else if (command == fCmdExternalConf) {
    fSLArAction->SetExternalConf(newValue); 
  }
  else if (command == fCmdGenieInput) {
    fSLArAction->SetGenieInput(newValue); 
  }
  else if (command == fCmdDirectionMode) {
    if (G4StrUtil::contains(newValue, "fixed")) {
      fSLArAction->SetDirectionMode(kFixed); 
    } else if (
        G4StrUtil::contains(newValue, "random") || 
        G4StrUtil::contains(newValue, "isotropic")) 
    {
      fSLArAction->SetDirectionMode(kRandom);
    } else {
      G4cout << "WARNING: unknown key " << newValue
        << ". I will assume you want it isotropic" << G4endl; 
      fSLArAction->SetDirectionMode(kRandom);
    }
  }
  else if (command == fCmdGunPosition) {
    G4ThreeVector pos = fCmdGunPosition->GetNew3VectorValue(newValue); 
    fSLArAction->SetSourcePosition( pos ); 
  }
  else if (command == fCmdEnergy) {
    G4double ekin = fCmdEnergy->GetNewDoubleValue(newValue); 
    fSLArAction->SetPGunEnergy(ekin);  
  }
  else if (command == fCmdGunDirection) {
    G4ThreeVector dir = fCmdGunDirection->GetNew3VectorValue(newValue); 
    fSLArAction->SetEventDirection(dir); 
  }
  else if (command == fCmdTracePhotons) {
    bool do_trace = fCmdTracePhotons->GetNewBoolValue(newValue); 
    fSLArAction->SetTraceOptPhotons(do_trace); 
  }
  else if (command == fCmdDriftElectrons) {
    bool do_drift = fCmdDriftElectrons->GetNewBoolValue(newValue); 
    fSLArAction->SetDriftElectrons(do_drift); 
  }



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
