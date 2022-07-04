/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQConstants.h
 * @created     : lunedì giu 27, 2022 18:48:44 CEST
 */

#ifndef SLARQCONSTANTS_H

#define SLARQCONSTANTS_H
//***********************************************
// GLOBAL VARIABLES
const double W      = 23.6*1e-6    ;// MeV/pair
const double Vdrift = 1.601        ;// mm/μs
const double Dlong  = 6.6270*1e-4  ;// mm2/μs
const double Dtrns  = 13.2327*1e-4 ;// mm2/μs
const double Elifetime = 2.9931*1e3;// μs

namespace slarq {
  const double xshift[3] = {600., 1000., 1500.}; 
}
#endif /* end of include guard SLARQCONSTANTS_H */

