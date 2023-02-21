/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArPGunGeneratorAction.hh
 * @created     Mon Jan 02, 2023 15:49:57 CET
 */

#ifndef SLARPGUNGENERATORACTION_HH

#define SLARPGUNGENERATORACTION_HH

#include <string>
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
class G4ParticleTable; 

class SLArPGunGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public: 
    SLArPGunGeneratorAction(); 
    SLArPGunGeneratorAction(const int n_particle); 
    ~SLArPGunGeneratorAction(); 

    inline void SetParticleMomentumDirection(const G4ThreeVector dir) 
      {fParticleGun->SetParticleMomentumDirection(dir);} 
    inline void SetParticleKineticEnergy(const G4double Ekin)
      {fParticleGun->SetParticleEnergy(Ekin);}
    inline void SetParticlePosition(const G4ThreeVector pos) 
      {fParticleGun->SetParticlePosition(pos);}
    inline void SetParticleTime(const G4double time) 
      {fParticleGun->SetParticleTime(time);}
    void SetParticle(const char* particle_name); 
    void SetParticle(G4ParticleDefinition* particle_def); 
    void GeneratePrimaries(G4Event*) override; 

  protected: 
    G4ParticleGun* fParticleGun; 
    G4ParticleTable* fParticleTable; 

}; 

#endif /* end of include guard SLARPGUNGENERATORACTION_HH */

