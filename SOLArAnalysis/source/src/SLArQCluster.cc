/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQCluster.cc
 * @created     : Mon Jun 27, 2022 15:15:25 CEST
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

  void  SLArQCluster::set_cluster_hist(THnBase* hn) {
    double xx[3] = {1e10};
    for (const auto &point : fPoints) {
      xx[0] = point.fPos.x(); 
      xx[1] = point.fPos.y(); 
      xx[2] = point.fPos.z();  
      hn->Fill(xx, point.fCharge);
    }

    adjust_h_range(hn, 0.); 

    return;
  }

  void adjust_h_range(THnBase* hn, double threshold) {
    int xmin[3]; int xmax[3]; 
    for (int ik=0; ik<hn->GetNdimensions(); ik++) {
      TH1D* h_tmp = hn->Projection(ik);
      xmin[ik] = h_tmp->GetXaxis()->GetBinLowEdge(h_tmp->FindFirstBinAbove(threshold));
      xmax[ik] = h_tmp->GetXaxis()->GetBinUpEdge (h_tmp->FindLastBinAbove (threshold));

      hn->GetAxis(ik)->SetRangeUser(xmin[ik]-1,xmax[ik]+1);
      delete h_tmp;
    }
    return;
  }
}

