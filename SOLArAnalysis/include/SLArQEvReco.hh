/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQEvReco
 * @created     : mercoledì giu 29, 2022 08:30:06 CEST
 */

#ifndef SLARQEVRECO_HH

#define SLARQEVRECO_HH

#include "SLArQReadout.hh"
#include "SLArQCluster.hh"

#include "TPrincipal.h"
#include "TH2D.h"
#include "TProfile.h"

namespace slarq {
  class  SLArQEvReco {
    public: 
      SLArQEvReco(); 
      SLArQEvReco(SLArQReadout* qev); 
      ~SLArQEvReco(); 

    protected:

  };
}

#endif /* end of include guard SLARQEVRECO_HH */

