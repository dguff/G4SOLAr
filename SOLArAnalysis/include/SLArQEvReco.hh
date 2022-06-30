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
#include <Math/DisplacementVector3D.h>
#include <Math/RotationZYX.h>
#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>
#include <Math/Vector3D.h>

namespace slarq {
  class SLArQBlipPCA : public TPrincipal {
    public: 
      SLArQBlipPCA(); 
      SLArQBlipPCA(int N, const char* opt="ND");

      ROOT::Math::RotationZYX GetRotation(); 
  };



  class  SLArQEvReco {
    public: 
      SLArQEvReco(); 
      SLArQEvReco(SLArQReadout* qev); 
      ~SLArQEvReco(); 

      void LoadQEv(SLArQReadout* qev) {fQev = qev;}

      void PCA(); 

      std::vector<TF1*>& ClusterFit(THnBase* h);
      double GetCosAngle(ROOT::Math::XYZVectorD); 
      std::vector<TF1*>& GetProjectionLinearFit() {return fProjFit;}


      void Reset(); 
      THnBase* RotateCluster(ROOT::Math::RotationZYX rot, ROOT::Math::XYZPointD anchor);

      void SetVertex(double *xvertex); 
      void SetVertex(ROOT::Math::XYZPointD xvertex); 

    protected:
      SLArQReadout* fQev; 
      SLArQBlipPCA* fPCA; 
      std::vector<TF1*> fProjFit;

      ROOT::Math::XYZPointD fVertex;

      int find_direction(TH2* h2, double xpos); 

  };

}



#endif /* end of include guard SLARQEVRECO_HH */

