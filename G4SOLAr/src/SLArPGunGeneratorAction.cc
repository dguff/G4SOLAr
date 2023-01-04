/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArPGunGeneratorAction.cc
 * @created     Mon Jan 02, 2023 15:52:56 CET
 */

#include "SLArPGunGeneratorAction.hh"
#include "G4ParticlePropertyTable.hh"

SLArPGunGeneratorAction::SLArPGunGeneratorAction() : fParticleGun(nullptr)
{
  fParticleGun = new G4ParticleGun(); 
  fParticleTable = G4ParticleTable::GetParticleTable(); 
}

SLArPGunGeneratorAction::SLArPGunGeneratorAction(const int n_particle) 
  : fParticleGun(nullptr)
{
  fParticleGun = new G4ParticleGun(n_particle); 
  fParticleTable = G4ParticleTable::GetParticleTable(); 
}


void SLArPGunGeneratorAction::SetParticle(const char* particle_name) {
  G4ParticleDefinition* particle = fParticleTable->FindParticle(particle_name); 
  SetParticle(particle); 
}

void SLArPGunGeneratorAction::SetParticle(G4ParticleDefinition* particle_def) 
{
  if (particle_def) {
    fParticleGun->SetParticleDefinition(particle_def); 
    return;
  } else {
    printf("SLArPGunGeneratorAction::SetParticle "); 
    printf("ERROR: cannot find %s in particle table.\n", 
        particle_def->GetParticleName().c_str()); 
    return;
  }
}

void SLArPGunGeneratorAction::GeneratePrimaries(G4Event* anEvent) 
{
  fParticleGun->GeneratePrimaryVertex(anEvent); 
}

SLArPGunGeneratorAction::~SLArPGunGeneratorAction()
{
  delete fParticleGun; 
}

