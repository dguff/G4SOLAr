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
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RandomTools.hh"

#include "marley/Event.hh"
#include "marley/Particle.hh"
#include "marley/RootJSONConfig.hh"

#include "SLArMarleyGeneratorAction.hh"

namespace marley {
SLArMarleyGeneratorAction::SLArMarleyGeneratorAction() 
  : G4VUserPrimaryGeneratorAction(), marley_vertex_generator_(0), marley_nu_direction(0, 0, 1) 
{
  fHalfLifeTable = {
        //{ 0.0298299*CLHEP::MeV, 4.25*CLHEP::ns },
        //{ 0.800143*CLHEP::MeV, 0.26*CLHEP::ps },
        { 1.64364*CLHEP::MeV, 0.336*CLHEP::us },
        //{ 1.95907*CLHEP::MeV, 0.54*CLHEP::ps },
        //{ 2.010368*CLHEP::MeV, 0.32*CLHEP::ps },
    };
}

SLArMarleyGeneratorAction::SLArMarleyGeneratorAction(
  const std::string& config_file_name) 
  : SLArMarleyGeneratorAction()
{
  // Create a new marley::Generator object using the settings from the
  // configuration file. If the USE_ROOT preprocessor macro is defined, then
  // parse the configuration file using a RootJSONConfig object to make
  // ROOT-dependent configuration options available (e.g., the use of a "th1"
  // or "tgraph" neutrino source)
  marley::RootJSONConfig config( config_file_name );
  marley_generator_= config.create_generator();
}

void SLArMarleyGeneratorAction::SetupMarleyGen(const std::string& config_file_name) 
{
  marley::RootJSONConfig config( config_file_name );
  marley_generator_= config.create_generator();
}

void SLArMarleyGeneratorAction::SetVertexGenerator(bxdecay0_g4::VertexGeneratorInterface* vtx_gen)
{
  marley_vertex_generator_ = vtx_gen; 
  return;
}

double SLArMarleyGeneratorAction::SampleDecayTime(const double half_life) const  {
  return CLHEP::RandExponential::shoot( half_life / log(2) ); 
}

void SLArMarleyGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // Create a new primary vertex at the spacetime origin.
  G4ThreeVector vtx(0., 0., 0); 
  if (marley_vertex_generator_) {
    marley_vertex_generator_->ShootVertex(vtx); 
  }

  

  G4double marley_time = 0.; 

  std::array<double, 3> dir = 
  {marley_nu_direction.x(), marley_nu_direction.y(), marley_nu_direction.z()};
  marley_generator_.set_neutrino_direction(dir); 

  // Generate a new MARLEY event using the owned marley::Generator object
  marley::Event ev = marley_generator_.create_event();

  // Get nuclear cascade info
  const auto& marley_cascades = ev.get_cascade_levels(); 
  const int marley_residue_pdg = ev.residue().pdg_code(); 

  // Loop over each of the final particles in the MARLEY event
  size_t particle_idx = 0; 
  size_t cascade_idx = 0; 
  std::vector<G4PrimaryVertex*> primary_vertices;

  for ( const auto& fp : ev.get_final_particles() ) {

    // Convert each one from a marley::Particle into a G4PrimaryParticle.
    // Do this by first setting the PDG code and the 4-momentum components.
    G4PrimaryParticle* particle = new G4PrimaryParticle( fp->pdg_code(),
      fp->px(), fp->py(), fp->pz(), fp->total_energy() );
    auto vertex = new G4PrimaryVertex(vtx, 0.); 

    // Also set the charge of the G4PrimaryParticle appropriately
    particle->SetCharge( fp->charge() );


    if (particle_idx > 1 && 
        marley_residue_pdg == 1000190400 && 
        cascade_idx < ev.get_cascade_levels().size()) {
      const auto& excited_state = marley_cascades[cascade_idx]->energy();

      auto iter = fHalfLifeTable.find(excited_state);
      if (iter != fHalfLifeTable.end()) {
        double decay_time = SampleDecayTime(iter->second);
        vertex->SetT0(marley_time + decay_time);
        std::cout << "Decay time: " << decay_time << std::endl;
      }
      cascade_idx++;
    }
    
    vertex->SetPrimary(particle); 
    primary_vertices.push_back(vertex); 

    // Add the fully-initialized G4PrimaryParticle to the primary vertex
    particle_idx++; 
  }


  // The primary vertex has been fully populated with all final-state particles
  // from the MARLEY event. Add it to the G4Event object so that Geant4 can
  // begin tracking the particles through the simulated geometry.
  for (const auto& vertex : primary_vertices) {
    anEvent->AddPrimaryVertex( vertex );
  }
}
}
