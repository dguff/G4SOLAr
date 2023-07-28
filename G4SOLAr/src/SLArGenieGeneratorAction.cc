/// @file
/// @copyright Copyright (C) 2016-2021 Steven Gardiner
/// @license GNU General Public License, version 3
//
// This file is part of MARLEY (Model of Argon Reaction Low Energy Yields)
//
// MARLEY is free software: you can redistribute it and/or modify it under the
// terms of version 3 of the GNU General Public License as published by the
// Free Software Foundation.
//
// For the full text of the license please see COPYING or
// visit http://opensource.org/licenses/GPL-3.0
//
// Please respect the MCnet academic usage guidelines. See GUIDELINES
// or visit https://www.montecarlonet.org/GUIDELINES for details.
// 
// Reimplemented in SoLAr-sim - SoLAr collaboration
// @author      Daniele Guffanti (University & INFN Milano-Bicocca), Nicholas Lane (University of Manchester)

#include <iostream>

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "SLArGenieGeneratorAction.hh"

#include "TFile.h"
#include "TTree.h"


SLArGenieGeneratorAction::SLArGenieGeneratorAction() 
  : G4VUserPrimaryGeneratorAction(), fIdx(0), fTree(nullptr)
{
}

SLArGenieGeneratorAction::SLArGenieGeneratorAction(
  const std::string& input_file_name) 
  : SLArGenieGeneratorAction()
{
	load_tree(input_file_name.c_str());
}

SLArGenieGeneratorAction::~SLArGenieGeneratorAction()
{
	std::cout << "destructor" << std::endl;
}

void SLArGenieGeneratorAction::load_tree(const char* ext_file_path) {

  TFile* fin = TFile::Open(ext_file_path);
	fTree = (TTree*) fin -> Get("enubetG"); // HARD CODED!!!!
	
	fTree->SetBranchAddress("EvtCode", &fGenieEvent.process);
	fTree->SetBranchAddress("StdHepN", &fGenieEvent.Npart);
	fTree->SetBranchAddress("StdHepPdg", &fGenieEvent.pdg);
	fTree->SetBranchAddress("StdHepP4", &fGenieEvent.momentum);
	fTree->SetBranchAddress("EvtVtx", &fGenieEvent.vertex);
	fTree->SetBranchAddress("StdHepStatus", &fGenieEvent.status);

	fTree->SetBranchStatus("*", 0);
	fTree->SetBranchStatus("Evt*", 1);
	fTree->SetBranchStatus("StdHep*", 1);
	
}

void SLArGenieGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  
  int e = anEvent->GetEventID();
  
  fTree->GetEntry(e);  
  
  std::cout << "evt = " << e << std::endl;

	//GenieParticle evt = genie_particles.at(e);
	
	GenieEvent evt = fGenieEvent;
  
  // Create a new primary vertex at the spacetime origin.
  G4ThreeVector vtx(0., 0., 0); 
  vtx.set(evt.vertex[0] * 1e-3, evt.vertex[1] * 1e-3, evt.vertex[2] * 1e-3); // from m to mm

/*
  std::array<double, 3> dir = 
  {marley_nu_direction.x(), marley_nu_direction.y(), marley_nu_direction.z()};
  marley_generator_.set_neutrino_direction(dir); */

  // Loop over each of the final particles in the Genie event
  size_t particle_idx = 0; 
  std::vector<G4PrimaryVertex*> primary_vertices;
  
  for (int j = 0; j < evt.Npart; j++) {

    // Create a G4PrimaryParticle by setting the PDG code and the 4-momentum   
    G4PrimaryParticle* particle = new G4PrimaryParticle(evt.pdg[j],
      evt.momentum[j][0] *1e3, evt.momentum[j][1] *1e3, evt.momentum[j][2] *1e3, evt.momentum[j][3] *1e3); // from GeV to MeV
    auto vertex = new G4PrimaryVertex(vtx, evt.vertex[3]);

    // Also set the charge of the G4PrimaryParticle appropriately
    //particle->SetCharge( fp->charge() );
    
    vertex->SetPrimary(particle); 
    primary_vertices.push_back(vertex); 

    // Add the fully-initialized G4PrimaryParticle to the primary vertex
    particle_idx++; 
  } 

  // The primary vertex has been fully populated with all final-state particles
  // from the Genie event. Add it to the G4Event object so that Geant4 can
  // begin tracking the particles through the simulated geometry.
  for (const auto& vertex : primary_vertices) {
    anEvent->AddPrimaryVertex( vertex );
  }
}
