/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArBackgroundGeneratorAction.cc
 * @created     Wed Jan 11, 2023 13:43:00 CET
 */

#include <stdio.h>

#include "rapidjson/document.h"
#include "rapidjson/allocators.h"
#include "rapidjson/encodings.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"

#include "G4PhysicalVolumeStore.hh"
#include "G4RandomTools.hh"
#include "G4Poisson.hh"
#include "G4Event.hh"

#include "bxdecay0_g4/primary_generator_action.hh"

#include "SLArBackgroundGeneratorAction.hh"
#include "SLArBulkVertexGenerator.hh"

SLArBackgroundGeneratorAction::SLArBackgroundGeneratorAction() : 
  fPrimaryGenAction(nullptr), fDecay0GenAction(nullptr), fBulkVtxGen(nullptr),
  fTime0(-0.2e6), fTime1(1.8e6) {}

SLArBackgroundGeneratorAction::SLArBackgroundGeneratorAction(const char* bkg_cfg_path) :
  fPrimaryGenAction(nullptr), fDecay0GenAction(nullptr), fBulkVtxGen(nullptr),
  fTime0(-0.2e6), fTime1(1.8e6)
{
  BuildBackgroundTable(bkg_cfg_path); 
}

SLArBackgroundGeneratorAction::~SLArBackgroundGeneratorAction() {}

void SLArBackgroundGeneratorAction::BuildBackgroundTable(const char* bkg_cfg_path)
{
  FILE* bkg_cfg_file = std::fopen(bkg_cfg_path, "r");
  char readBuffer[65536];
  rapidjson::FileReadStream is(bkg_cfg_file, readBuffer, sizeof(readBuffer));

  rapidjson::Document d;
  d.ParseStream<rapidjson::kParseCommentsFlag>(is);

  assert(d.HasMember("backgrounds")); 
  assert(d["backgrounds"].IsArray()); 

  size_t ibkg = 1; 
  for (const auto &b : d["backgrounds"].GetArray()) {
    background::SLArBackgroundInfo bkg(
          b["isotope"].GetString(), 
          b["volume" ].GetString(), 
          b["rate"   ].GetDouble()
        ); 

    fBkgModel.insert(std::make_pair(ibkg, bkg)); 
    ibkg++;
  }

  std::fclose(bkg_cfg_file); 
}

void SLArBackgroundGeneratorAction::GeneratePrimaries(G4Event* ev) 
{
#ifdef SLAR_DEBUG
  printf("SLArBackgroundGeneratorAction::GeneratePrimaries\n");
#endif
  fPrimaryGenAction->GeneratePrimaries(ev); 

  for (const auto &bkg : fBkgModel) {
    bxdecay0_g4::PrimaryGeneratorAction::ConfigurationInterface& decay0_cfg =
      fDecay0GenAction->GrabConfiguration(); 
    decay0_cfg.reset_base(); 
    decay0_cfg.decay_category = "background";
    decay0_cfg.nuclide = bkg.second.fIsotope;
    decay0_cfg.seed = 1e6*G4UniformRand();
    fDecay0GenAction->SetConfigHasChanged(true);
    if (fDecay0GenAction->IsDebug()) {
      std::cerr << "[debug] bxdecay0_g4::PrimaryGeneratorActionMessenger::SetNewValue: "
        << "Command 'background' -> Action -> update the working configuration...\n";
      decay0_cfg.print(std::cerr);
    }
    SLArBulkVertexGenerator* vtx_gen = new SLArBulkVertexGenerator(*fBulkVtxGen); 
    fDecay0GenAction->SetVertexGenerator(vtx_gen); 

    G4double mass = SetBackgroundVolume(vtx_gen, bkg.second.fVolume); 
    G4double n_decay_expected = 
      (fTime1 - fTime0)/CLHEP::second * mass * bkg.second.fRate;
    printf("mass = %g kg, expected %g events in %g ms\n", 
        mass, n_decay_expected, (fTime1-fTime0)/CLHEP::ms);
    G4int n_decay = G4Poisson(n_decay_expected);

#ifdef SLAR_DEBUG
    printf("Producing %i %s decays...\n", n_decay, bkg.second.fIsotope.c_str());
#endif

    for (int i=0; i<n_decay; i++) {
      fDecay0GenAction->GeneratePrimaries(ev); 
    }

  } 

  return;
}

G4double SLArBackgroundGeneratorAction::SetBackgroundVolume(
    SLArBulkVertexGenerator* vtx_gen, G4String vol) {
  printf("SLArBackgroundGeneratorAction::SetBackgroundVolume(%s)\n", vol.c_str());
  auto volume = G4PhysicalVolumeStore::GetInstance()->GetVolume(vol); 
  if (volume == nullptr) {
    printf("SLArBackgroundGeneratorAction::SetBackgroundVolume(%s) WARNING\n", vol.c_str());
    printf("Unable to find %s in physical volume store.\n", vol.c_str());
  }

  vtx_gen->SetBulkLogicalVolume(volume->GetLogicalVolume()); 
  vtx_gen->SetSolidTranslation(volume->GetTranslation()); 
  vtx_gen->SetSolidRotation(volume->GetRotation()); 
  return volume->GetLogicalVolume()->GetMass()/CLHEP::kg;
}


void SLArBackgroundGeneratorAction::PrintBackgroundModel() {
  printf("SLArBackgroundGeneratorAction Background Model\n");
  printf("--------------------------------------------------\n");
  printf("Isotope      | Volume      | Rate [Bq/kg]\n");
  for (const auto& bkg : fBkgModel) {
    printf("%s         | %s       | %g\n", 
        bkg.second.fIsotope.c_str(), bkg.second.fVolume.c_str(), bkg.second.fRate);
  }
  printf("--------------------------------------------------\n");
  return;
}
