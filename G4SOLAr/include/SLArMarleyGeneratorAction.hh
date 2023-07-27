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

#pragma once

// Standard library includes
#include <string>
#include <map>

// Geant4 includes
#include "G4VUserPrimaryGeneratorAction.hh"

#include "bxdecay0_g4/vertex_generator_interface.hh"

// MARLEY includes
#include "marley/Generator.hh"

class G4Event;

namespace marley {

class SLArMarleyGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    SLArMarleyGeneratorAction();
    SLArMarleyGeneratorAction(const std::string& config_file_name);

    void SetupMarleyGen(const std::string& config_file_name); 
    void SetVertexGenerator(bxdecay0_g4::VertexGeneratorInterface*); 
    virtual void GeneratePrimaries(G4Event*) override;
    void SetNuDirection(G4ThreeVector dir) {marley_nu_direction = dir;} 
    G4ThreeVector GetNuDirection() {return marley_nu_direction;}
    inline G4double GetFluxAveragedTotalXSec() const {return marley_generator_.flux_averaged_total_xs();}

  protected:
    // MARLEY event generator object
    marley::Generator marley_generator_;
    bxdecay0_g4::VertexGeneratorInterface* marley_vertex_generator_;
    G4ThreeVector marley_nu_direction; 
    double SampleDecayTime(const double half_life) const;
    std::map<double, double> fHalfLifeTable;
};
}
