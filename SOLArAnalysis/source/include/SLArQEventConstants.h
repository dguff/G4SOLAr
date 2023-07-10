/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQEventConstants.h
 * @created     : Mon Jun 27, 2022 18:48:44 CEST
 */

#ifndef SLARQEVENTCONSTANTS_H

#define SLARQEVENTCONSTANTS_H

//***********************************************
// GLOBAL VARIABLES
const double W         = 23.6*1e-6 ;// MeV/pair
const double Vdrift    = 1.601e-3  ;// mm/ns
const double Elifetime = 10*1e6    ;// ns

namespace slarq {
  const double xshift[3] = {300., 1000., 1500.}; 
}

#endif /* end of include guard SLARQEVENTCONSTANTS_H */
