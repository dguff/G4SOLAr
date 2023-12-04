/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArExternalGeneratorAction.hh
 * @created     Tue Apr 11, 2023 09:43:54 CEST
 */

#ifndef SLAREXTERNALGENERATORACTION_HH

#define SLAREXTERNALGENERATORACTION_HH

#include <string>

#include "SLArBoxSurfaceVertexGenerator.hh"
#include "SLArPGunGeneratorAction.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

#include "TH1D.h"
#include "TRandom3.h"

class G4ParticleTable;

class SLArExternalGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public: 
    SLArExternalGeneratorAction(); 
    SLArExternalGeneratorAction(const char* ext_cfg_path); 
    virtual ~SLArExternalGeneratorAction(); 

    virtual void GeneratePrimaries(G4Event* ev); 
    void SetVertexGenerator(SLArBoxSurfaceVertexGenerator* vtxGen) {fVtxGen  = vtxGen;}
    SLArBoxSurfaceVertexGenerator* GetVertexGenerator() {return fVtxGen;}
    G4double SourceExternalConfig(const char* ext_cfg_path); 
    G4double SetGeneratorBox(const G4String volName); 

  protected:
    SLArPGunGeneratorAction* fParticleGun; 
    SLArBoxSurfaceVertexGenerator* fVtxGen;
    TH1D* fEnergySpectrum; 
    G4String fParticleName; 
    TRandom3* fRandomEngine; 

}; 

#endif /* end of include guard SLAREXTERNALGENERATORACTION_HH */

