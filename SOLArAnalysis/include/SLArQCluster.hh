/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQCluster
 * @created     : lunedì giu 27, 2022 15:07:45 CEST
 */

#ifndef SLARQCLUSTER_HH

#define SLARQCLUSTER_HH

#include <iostream>
#include <vector>
#include "TObject.h"
#include "TMath.h"
#include "Math/DisplacementVector3D.h"
#include "Math/Vector3D.h"
#include "Math/Point3D.h"
#include "Math/RotationZYX.h"
#include "TH3D.h"
#include "THnBase.h"

namespace slarq {
  typedef ROOT::Math::XYZPointF SLArQSpatialPoint;

  class cluster_point : public TObject {
    public: 
      SLArQSpatialPoint fPos; 
      Long64_t  fBin; 
      float     fCharge; 

      cluster_point() {}; 
      cluster_point(const cluster_point& pt) {
        fPos = pt.fPos; 
        fBin = pt.fBin; 
        fCharge = pt.fCharge;
      }

      ClassDef(cluster_point, 1);
  };

  void adjust_h_range(THnBase*, double threshold); 


  class SLArQCluster : public TObject {
    public: 
      SLArQCluster();
      SLArQCluster(size_t id); 
      SLArQCluster(const SLArQCluster& cluster);
      ~SLArQCluster();

      double get_charge() {return fCharge;}
      size_t get_id() {return fID;}
      std::vector<cluster_point>& get_points() {return fPoints;}

      bool is_registered(Long64_t ibin); 

      void set_cluster_hist(TH3* h);
      void set_cluster_hist(THnBase* hn);
      void register_point(double* x, int ibin, double q);
      void register_point(cluster_point point);

    protected:
      size_t fID; 
      double fCharge;
      std::vector<cluster_point> fPoints;


    public:
      ClassDef(slarq::SLArQCluster, 1)
  };

}



#endif /* end of include guard SLARQCLUSTER_HH */

