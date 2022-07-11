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

// Standard library includes
#include <iostream>

// Geant4 includes
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

// MARLEY includes
#include "marley/Event.hh"
#include "marley/Particle.hh"
#include "marley/RootJSONConfig.hh"
// marg4 includes
#include "SLArMarleyGen.hh"

SLArMarleyGen::SLArMarleyGen(
  const std::string& config_file_name) 
  : G4VUserPrimaryGeneratorAction(), marley_vertex_generator_(0), marley_nu_direction(0, 0, 1) 
{
  // Create a new marley::Generator object using the settings from the
  // configuration file. If the USE_ROOT preprocessor macro is defined, then
  // parse the configuration file using a RootJSONConfig object to make
  // ROOT-dependent configuration options available (e.g., the use of a "th1"
  // or "tgraph" neutrino source)
  marley::RootJSONConfig config( config_file_name );
  marley_generator_= config.create_generator();
}

void SLArMarleyGen::SetVertexGenerator(bxdecay0_g4::VertexGeneratorInterface* vtx_gen)
{
  marley_vertex_generator_ = vtx_gen; 
  return;
}

void SLArMarleyGen::GeneratePrimaries(G4Event* anEvent)
{
  // Create a new primary vertex at the spacetime origin.
  G4ThreeVector vtx(0., 0., 0); 
  if (marley_vertex_generator_) {
    marley_vertex_generator_->ShootVertex(vtx); 
  }

  G4PrimaryVertex* vertex = new G4PrimaryVertex(vtx, 0.); 

  std::array<double, 3> dir = 
  {marley_nu_direction.x(), marley_nu_direction.y(), marley_nu_direction.z()};
  marley_generator_.set_neutrino_direction(dir); 

  // Generate a new MARLEY event using the owned marley::Generator object
  marley::Event ev = marley_generator_.create_event();

  // This line, if uncommented, will print the event in ASCII format
  // to standard output
  //std::cout << ev << '\n';

  // Loop over each of the final particles in the MARLEY event
  for ( const auto& fp : ev.get_final_particles() ) {

    // Convert each one from a marley::Particle into a G4PrimaryParticle.
    // Do this by first setting the PDG code and the 4-momentum components.
    G4PrimaryParticle* particle = new G4PrimaryParticle( fp->pdg_code(),
      fp->px(), fp->py(), fp->pz(), fp->total_energy() );

    // Also set the charge of the G4PrimaryParticle appropriately
    particle->SetCharge( fp->charge() );

    // Add the fully-initialized G4PrimaryParticle to the primary vertex
    vertex->SetPrimary( particle );
  }


  // The primary vertex has been fully populated with all final-state particles
  // from the MARLEY event. Add it to the G4Event object so that Geant4 can
  // begin tracking the particles through the simulated geometry.
  anEvent->AddPrimaryVertex( vertex );
}
