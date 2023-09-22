/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArPBombGeneratorAction
 * @created     Friday Sep 22, 2023 09:17:19 CEST
 */

#ifndef SLARPBOMBGENERATORACTION_HH

#define SLARPBOMBGENERATORACTION_HH

#include <string>
#include "G4VUserPrimaryGeneratorAction.hh"
#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>
class G4ParticleTable; 

class SLArPBombGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public: 
    SLArPBombGeneratorAction(); 
    SLArPBombGeneratorAction(const int n_particle); 
    ~SLArPBombGeneratorAction(); 

    inline void SetParticleMomentumDirection(const G4ThreeVector dir) 
      {fDirection = dir;} 
    inline void SetParticleKineticEnergy(const G4double Ekin)
      {fParticleEnergy = Ekin;}
    inline void SetParticlePosition(const G4ThreeVector pos) 
      {fVertex = pos;}
    inline void SetParticleTime(const G4double time) 
      {fTime = time;}
    inline void SetNumberOfParticles(const G4int n_particles) 
      {fNumberOfParticles = n_particles;} 
    void SetParticle(const char* particle_name); 
    void SetParticle(G4ParticleDefinition* particle_def); 
    void GeneratePrimaries(G4Event*) override; 

  protected: 
    G4ThreeVector fDirection;
    G4double fParticleEnergy; 
    G4ThreeVector fVertex; 
    G4double fTime; 
    G4int fNumberOfParticles;
    G4ParticleDefinition* fParticleDefinition; 
    G4ParticleTable* fParticleTable; 

}; 

#endif /* end of include guard SLARPBOMBGENERATORACTION_HH */

