/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArIonAndScintModel.cc
 * @created     Wed Mar 01, 2023 11:41:24 CET
 */

#include "physics/SLArIonAndScintModel.h"
#include "G4MaterialPropertiesTable.hh"
#include "G4SystemOfUnits.hh"

SLArIonAndScintModel::SLArIonAndScintModel() :
  fWion(23.6e-6), fBirksK(0.0486), fBirksA(0.800), fLArDensity(1.39)
{
  fIonizationDensity = 1.0 / fWion;
}

SLArIonAndScintModel::SLArIonAndScintModel(const G4MaterialPropertiesTable* mpt) :
  fWion(mpt->GetConstProperty("WION")), 
  fBirksK(mpt->GetConstProperty("BIRKSRECOMBK")),
  fBirksA(mpt->GetConstProperty("BIRKSRECOMBA")), 
  fLArDensity(mpt->GetConstProperty("DENSITY")/( CLHEP::g / CLHEP::cm3 ))
{
  fIonizationDensity = 1.0 / fWion;   
}

double SLArIonAndScintModel::ComputeIon(const double energy_deposit, const double step_length, const double electric_field) const {
  const double yield = ComputeIonYield(energy_deposit, step_length, electric_field); 
  return energy_deposit * yield; 
}

double SLArIonAndScintModel::ComputeScint(const double energy_deposit, const double step_length, const double electric_field) const {
  const double yield = ComputeScintYield(energy_deposit, step_length, electric_field); 
  return energy_deposit * yield; 
}
