/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArPBombGeneratorAction
 * @created     Friday Sep 22, 2023 09:18:51 CEST
 */

#include "SLArPBombGeneratorAction.hh"
#include "SLArRandomExtra.hh"
#include "G4ParticlePropertyTable.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"

SLArPBombGeneratorAction::SLArPBombGeneratorAction() : 
  fDirection( G4ThreeVector(0, 0, 0) ), fParticleEnergy(1e-6), 
  fVertex( G4ThreeVector(0, 0, 0) ), fTime(0.0), fNumberOfParticles(1)
{
  fParticleTable = G4ParticleTable::GetParticleTable(); 
}

SLArPBombGeneratorAction::SLArPBombGeneratorAction(const int n_particle) 
  : SLArPBombGeneratorAction()
{
  fNumberOfParticles = n_particle;
  fParticleTable = G4ParticleTable::GetParticleTable(); 
}


void SLArPBombGeneratorAction::SetParticle(const char* particle_name) {
  G4ParticleDefinition* particle = fParticleTable->FindParticle(particle_name); 
  SetParticle(particle); 
}

void SLArPBombGeneratorAction::SetParticle(G4ParticleDefinition* particle_def) 
{
  if (particle_def) {
    fParticleDefinition = particle_def;
    return;
  } else {
    printf("SLArPBombGeneratorAction::SetParticle "); 
    printf("ERROR: cannot find %s in particle table.\n", 
        particle_def->GetParticleName().c_str()); 
    return;
  }
}

void SLArPBombGeneratorAction::GeneratePrimaries(G4Event* anEvent) 
{
  G4PrimaryVertex* vertex = new G4PrimaryVertex( fVertex, fTime ); 
  G4ThreeVector dir(0, 0, 0); 

  bool is_dir_random = (fDirection == G4ThreeVector(0,0,0) );

  for (size_t n=0; n<fNumberOfParticles; n++) {
    G4PrimaryParticle* particle = new G4PrimaryParticle(fParticleDefinition);
    if (is_dir_random) {
      dir = SampleRandomDirection();
    }
    else {
      dir = fDirection;
    }
    particle->SetMomentumDirection( dir ); 
    particle->SetKineticEnergy( fParticleEnergy ); 

    vertex->SetPrimary( particle ) ; 
  }

  anEvent->AddPrimaryVertex(vertex); 
  return;
}

SLArPBombGeneratorAction::~SLArPBombGeneratorAction()
{}

