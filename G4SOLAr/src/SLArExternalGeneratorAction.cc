/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArExternalGeneratorAction.cc
 * @created     Tua Apr 11, 2023 09:44:12 CEST
 */

#include "SLArExternalGeneratorAction.hh"

#include <stdio.h>

#include "rapidjson/document.h"
#include "rapidjson/allocators.h"
#include "rapidjson/encodings.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"

#include "TFile.h"
#include "TH1D.h"
#include "TRandom3.h"

#include "G4PhysicalVolumeStore.hh"
#include "G4RandomTools.hh"
#include "G4Poisson.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"


SLArExternalGeneratorAction::SLArExternalGeneratorAction()
  : fParticleGun(nullptr), fVtxGen(nullptr), fEnergySpectrum(nullptr)
{
  fParticleGun = new SLArPGunGeneratorAction(1); 

  fRandomEngine = new TRandom3( G4Random::getTheSeed() ); 
}

SLArExternalGeneratorAction::~SLArExternalGeneratorAction()
{
  if (fParticleGun) {delete fParticleGun; fParticleGun=nullptr;}
  if (fEnergySpectrum) {delete fEnergySpectrum; fEnergySpectrum=nullptr;}
  if (fRandomEngine) {delete fRandomEngine; fRandomEngine=nullptr;}
}

void SLArExternalGeneratorAction::SourceExternalConfig(const char* ext_cfg_path) {
  FILE* ext_cfg_file = std::fopen(ext_cfg_path, "r");
  char readBuffer[65536];
  rapidjson::FileReadStream is(ext_cfg_file, readBuffer, sizeof(readBuffer));

  rapidjson::Document d;
  d.ParseStream<rapidjson::kParseCommentsFlag>(is);

  assert(d.HasMember("externals")); 
  assert(d["externals"].IsObject());

  auto external_cfg = d["externals"].GetObject(); 
  
  fParticleName = external_cfg["particle"].GetString(); 
  
  if (external_cfg.HasMember("origin_volume")) {
    if (fVtxGen) {
      SetGeneratorBox(external_cfg["origin_volume"].GetString()); 
    } else {
      printf("SLArExternalGeneratorAction::SourceExternalConfig() WARNING\n");
      printf("Cannot set the generator volume because the vertex generator is not assigned\n");
    }
  }

  if (external_cfg.HasMember("origin_face")) {
    if (fVtxGen) {
      fVtxGen->FixVertexFace(true); 
      fVtxGen->SetVertexFace(
          (slargeo::EBoxFace)external_cfg["origin_face"].GetInt()); 
    }
  }

  TFile* input_file = new TFile(external_cfg["file"].GetString()); 
  if (!input_file) {
    printf("SLArExternalGeneratorAction::SourceExternalConfig ERROR\n");
    printf("Cannot open external background file %s.\n", external_cfg["file"].GetString()); 
    getchar(); 
  }
  TH1D* h = (TH1D*)input_file->Get(external_cfg["key"].GetString()); 
  fEnergySpectrum = (TH1D*)h->Clone(); 
  //input_file->Close(); 
  if (!fEnergySpectrum) {
    printf("SLArExternalGeneratorAction::SourceExternalConfig ERROR\n");
    printf("Cannot read key %s from external background file %s.\n", 
        external_cfg["key"].GetString(), external_cfg["file"].GetString()); 
    getchar(); 
  }
}

G4double SLArExternalGeneratorAction::SetGeneratorBox(const G4String volName) {
  printf("SLArExternalGeneratorAction::SetGeneratorBox(%s)\n", volName.c_str());
  auto volume = G4PhysicalVolumeStore::GetInstance()->GetVolume(volName); 
  if (volume == nullptr) {
    printf("SLArBoxSurfaceVertexGenerator::SetGeneratorBox(%s) WARNING\n", 
        volName.c_str());
    printf("Unable to find %s in physical volume store.\n", volName.c_str());
    getchar(); 
  }

  fVtxGen->SetBoxLogicalVolume(volume->GetLogicalVolume()); 
  fVtxGen->SetSolidTranslation(volume->GetTranslation()); 
  fVtxGen->SetSolidRotation(volume->GetRotation()); 
  return volume->GetLogicalVolume()->GetMass()/CLHEP::kg;
}

void SLArExternalGeneratorAction::GeneratePrimaries(G4Event* ev) 
{
#ifdef SLAR_DEBUG
  printf("SLArExternalGeneratorAction::GeneratePrimaries\n");
#endif
  
  G4ThreeVector vtx_pos(0, 0, 0); 
  fVtxGen->ShootVertex(vtx_pos);
  
  //printf("Energy spectrum pointer: %p\n", static_cast<void*>(fEnergySpectrum));
  //printf("Energy spectrum from %s\n", fEnergySpectrum->GetName());
  G4double energy = fEnergySpectrum->GetRandom( fRandomEngine ); 
  auto face = fVtxGen->GetVertexFace(); 
  const auto face_normal = slargeo::BoxFaceNormal[face]; 
  //printf("SLArExternalGeneratorAction: vtx face is %i\n", face);
  //printf("SLArExternalGeneratorAction: face normal is [%.1f, %.1f, %.1f]\n", 
      //face_normal.x(), face_normal.y(), face_normal.z()); 

  G4ThreeVector dir = SampleRandomDirection(); 
  while ( dir.dot(face_normal) < 0 ) {
    dir = SampleRandomDirection(); 
  }

  //G4cout << "Momentum direction is: " << dir << G4endl; 
  
  fParticleGun->SetParticle(fParticleName); 
  fParticleGun->SetParticleMomentumDirection(dir); 
  fParticleGun->SetParticlePosition(vtx_pos); 
  fParticleGun->SetParticleKineticEnergy(energy); 
  fParticleGun->SetParticleTime(0); 

  fParticleGun->GeneratePrimaries(ev); 

  //getchar(); 

  return;
}

G4ThreeVector SLArExternalGeneratorAction::SampleRandomDirection() {
  double cosTheta = 2*G4UniformRand() - 1.;
  double phi = CLHEP::twopi*G4UniformRand();
  double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
  double ux = sinTheta*std::cos(phi),
         uy = sinTheta*std::sin(phi),
         uz = cosTheta;

  G4ThreeVector dir(ux, uy, uz);
  
  return dir; 
}

