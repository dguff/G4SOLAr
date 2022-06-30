/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQDiffusion
 * @created     : martedì giu 28, 2022 09:08:13 CEST
 */

#ifndef SLARQDIFFUSION_HH

#define SLARQDIFFUSION_HH

#include <iostream>
#include "TMath.h"
#include "TF1.h"
#include "TRandom3.h"

#include "SLArQReadout.hh"
#include "event/SLArEventTrajectory.hh"

#include "SLArQConstants.h"

namespace slarq {
  class SLArQDiffusion {
    public: 
      SLArQDiffusion(); 
      ~SLArQDiffusion();

      double DiffuseRandom(const trj_point*, SLArQReadout* qev); 
      double DiffuseAnalytical(const trj_point*, SLArQReadout* qev); 
    protected: 
      double Ldiffusion(double l); 
      double Tdiffusion(double l); 
  };
}


#endif /* end of include guard SLARQDIFFUSION_HH */

