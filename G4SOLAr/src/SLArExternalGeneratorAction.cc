/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArExternalGeneratorAction.cc
 * @created     Tua Apr 11, 2023 09:44:12 CEST
 */

#include "SLArExternalGeneratorAction.hh"
#include "SLArRandomExtra.hh"

#include <stdio.h>
#include <memory>

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
  : fVtxGen(nullptr)
{
  fParticleGun = std::make_unique<SLArPGunGeneratorAction>(1); 

  fRandomEngine = std::make_unique<TRandom3>( G4Random::getTheSeed() ); 
}

SLArExternalGeneratorAction::~SLArExternalGeneratorAction()
{}

G4double SLArExternalGeneratorAction::SourceExternalConfig(const G4String ext_cfg_path) {
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

  TFile input_file(external_cfg["file"].GetString()); 
  if (input_file.IsOpen() == false) {
    printf("SLArExternalGeneratorAction::SourceExternalConfig ERROR\n");
    printf("Cannot open external background file %s.\n", external_cfg["file"].GetString()); 
    exit(2); 
  }
  TH1D* h = input_file.Get<TH1D>(external_cfg["key"].GetString()); 
  h->SetDirectory( nullptr ); 
  input_file.Close(); 

  fEnergySpectrum = std::make_unique<TH1D>( *h ); 
  if (!fEnergySpectrum) {
    printf("SLArExternalGeneratorAction::SourceExternalConfig ERROR\n");
    printf("Cannot read key %s from external background file %s.\n", 
        external_cfg["key"].GetString(), external_cfg["file"].GetString()); 
    exit(2); 
  }

  fclose(ext_cfg_file); 
  delete h; 

  return fVtxGen->GetSurfaceGenerator(); 
}

G4double SLArExternalGeneratorAction::SetGeneratorBox(const G4String volName) {
  printf("SLArExternalGeneratorAction::SetGeneratorBox(%s)\n", volName.c_str());
  auto volume = G4PhysicalVolumeStore::GetInstance()->GetVolume(volName); 
  if (volume == nullptr) {
    printf("SLArBoxSurfaceVertexGenerator::SetGeneratorBox(%s) WARNING\n", 
        volName.c_str());
    printf("Unable to find %s in physical volume store.\n", volName.c_str());
    exit(2); 
  }

  fVtxGen->SetBoxLogicalVolume(volume->GetLogicalVolume()); 
  fVtxGen->SetSolidTranslation(volume->GetTranslation()); 
  fVtxGen->SetSolidRotation(volume->GetRotation()); 
  G4double area = 0; 
  if (const auto& box = dynamic_cast<G4Box*>(volume->GetLogicalVolume()->GetSolid())) {
    area = box->GetSurfaceArea(); 
  }
  return area/CLHEP::cm2;
}

void SLArExternalGeneratorAction::GeneratePrimaries(G4Event* ev) 
{
#ifdef SLAR_DEBUG
  printf("SLArExternalGeneratorAction::GeneratePrimaries\n");
#endif
  
  G4ThreeVector vtx_pos(0, 0, 0); 
  fVtxGen->ShootVertex(vtx_pos);
  
  printf("Energy spectrum pointer: %p\n", fEnergySpectrum.get());
  printf("Energy spectrum from %s\n", fEnergySpectrum->GetName());
  G4double energy = fEnergySpectrum->GetRandom( fRandomEngine.get() ); 
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


