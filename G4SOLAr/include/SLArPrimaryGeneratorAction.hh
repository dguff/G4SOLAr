// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file SLAr/include/SLArPrimaryGeneratorAction.hh
/// \brief Definition of the SLArPrimaryGeneratorAction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef SLArPrimaryGeneratorAction_h
#define SLArPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VPhysicalVolume.hh"
#include "globals.hh"

class G4Event;
class SLArBulkVertexGenerator;
class SLArBoxSurfaceVertexGenerator;
class SLArPrimaryGeneratorMessenger;
class SLArPGunGeneratorAction; 
class SLArBackgroundGeneratorAction;
class SLArExternalGeneratorAction;
class SLArNuInteractionGeneratorAction;

namespace bxdecay0_g4 {
  class SLArDecay0GeneratorAction;
}
namespace marley {
  class SLArMarleyGeneratorAction;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

enum  EGenerator {kParticleGun=0, kDecay0=1, kMarley=2, kBackground=3, kExternalGen=4, kGenie=5};
enum  EDirectionMode {kFixed = 0, kRandom = 1};

class SLArPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{

  public:
    SLArPrimaryGeneratorAction();
    virtual ~SLArPrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

    inline bool DoTraceOptPhotons() {return fDoTraceOptPhotons;}
    inline bool DoDriftElectrons() {return fDoDriftElectrons;}
    inline void SetTraceOptPhotons(bool do_trace) {fDoTraceOptPhotons = do_trace;}
    inline void SetDriftElectrons(bool do_drift) {fDoDriftElectrons = do_drift;}

    G4String GetMarleyConf() {return fMarleyCfg;}
    EDirectionMode GetDirectionMode() {return fDirectionMode;}
    void SetDirectionMode(EDirectionMode kMode) {fDirectionMode = kMode;}
    void SetGenerator(EGenerator gen) {fGeneratorEnum = gen;}
    void SetSourcePosition(G4ThreeVector pos) {fGunPosition = pos;}
    void SetEventDirection(G4ThreeVector dir) {fGunDirection = dir;}
    void SetBulkName(G4String vol);
    void SetBoxName(G4String vol); 
    void SetBackgroundConf(G4String background_conf); 
    void SetExternalConf(G4String externals_conf); 
    void SetMarleyConf(G4String marley_conf);
    void SetGenieInput(G4String genie_input);
    void SetGenieEventSeed(G4int event_id);
    void SetFirstEventID(G4Event* anEvent, G4int event_id);
    void SetPGunEnergy(G4double ekin); 
    void SetPGunParticle(G4String particle_name); 

  private:
    std::vector<G4VUserPrimaryGeneratorAction*> fGeneratorActions; 
    //SLArPGunGeneratorAction* fPGunGen;
    //bxdecay0_g4::PrimaryGeneratorAction* fDecay0Gen;
    //marley::SLArMarleyGeneratorAction*   fMarleyGen; 


    SLArPrimaryGeneratorMessenger* fGunMessenger;

    SLArBulkVertexGenerator* fBulkGenerator;
    SLArBoxSurfaceVertexGenerator* fBoxGenerator; 

    EDirectionMode fDirectionMode; 
    G4String       fVolumeName;
    G4String       fMarleyCfg; 
    G4String       fGenieCfg;
    G4int          fGenieEventSeed;

    EGenerator     fGeneratorEnum;
    G4ThreeVector  fGunPosition;
    G4ThreeVector  fGunDirection;
    G4double       fGunEnergy; 
    bool           fIncludeBackground; 
    G4String       fBackgoundModelCfg;

    G4bool fDoDriftElectrons;
    G4bool fDoTraceOptPhotons;

    G4ThreeVector  SampleRandomDirection(); 

    friend class SLArPrimaryGeneratorMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*SLArPrimaryGeneratorAction_h*/
