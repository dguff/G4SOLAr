/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQCluster
 * @created     : lunedì giu 27, 2022 15:15:25 CEST
 */

#include "SLArQCluster.hh"

ClassImp(slarq::SLArQCluster)

namespace slarq {

    SLArQCluster::SLArQCluster()
    : TObject(), fID(-1)
    {}

  SLArQCluster::SLArQCluster(size_t id) 
    : TObject(), fID(id) 
  {}

  SLArQCluster::~SLArQCluster()
  {}

  void SLArQCluster::register_point(double *x, int ibin, double q) {
    cluster_point point; 
    point.fPos.SetXYZ(x[0], x[1], x[2]); 
    point.fBin = ibin; 
    point.fCharge = q;

    fCharge += q;
    fPoints.push_back( cluster_point(point) );
  }

  void SLArQCluster::register_point(cluster_point point) {
    double xpoint[3]; 
    xpoint[0] = point.fPos.x(); 
    xpoint[1] = point.fPos.y(); 
    xpoint[2] = point.fPos.z(); 

    fCharge += point.fCharge;
    fPoints.push_back( cluster_point(point) );
  }

  bool SLArQCluster::is_registered(Long64_t ibin) {
    for (const auto &p : fPoints) {
      if (ibin == p.fBin) {
        return true;
      }
    } 

    return false;
  }

  void  SLArQCluster::set_cluster_hist(TH3* h) {
    for (const auto &point : fPoints) {
      h->Fill(
          point.fPos.x(), 
          point.fPos.y(), 
          point.fPos.z(),
          point.fCharge);
    }

    return;
  }
}

