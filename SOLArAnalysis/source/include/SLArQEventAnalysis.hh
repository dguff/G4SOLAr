/**
 * @author      Giulia Conti (unimib), Daniele Guffanti (unimib & infn-mib)
 * @file        SLArQEventAnalysis.hh
 * @created     Fri Jul 14, 2023 10:36:05 CEST
 */

#ifndef SLARQEVENTANALYSIS_HH

#define SLARQEVENTANALYSIS_HH

#include <iostream>
#include <vector>

#include "TGraphErrors.h"
#include "TF1.h"

#include "config/SLArCfgAnode.hh"
#include "SLArQEventReadout.hh"

namespace slarq {

struct cluster_projection_info_t {
  enum  EClusterDirMethod {kNHits = 0, kLength = 1, kCharge = 2};
  float fPar0 = 0; //!< linear fit p0
  float fPar1 = 0; //!< linear fit p1
  int   fDirX = 0; 
  int   fDirY = 0; 
  float fChargeX[2] = {0}; //!< cluster charge left[0] and right[1] of the vertex
  float fLengthX[2] = {0}; //!< cluster length left[0] and right[1] of the vertex
  float fNHitsX [2] = {0}; //!< cluster hits left[0] and right[1] of the vertex
  float fChargeY[2] = {0}; //!< cluster charge below[0] and above[1] of the vertex
  float fLengthY[2] = {0}; //!< cluster length below[0] and above[1] of the vertex
  float fNHitsY [2] = {0}; //!< cluster hits below[0] and above[1] of the vertex  
  TVector3 fAxis[2] = {TVector3()}; //!< projection local x[0] and y[1] axes
  int   fAxisIdx[2] = {0}; //!< projection local axes indices 
  inline cluster_projection_info_t() {}

  inline void set_dir_xy(EClusterDirMethod kMethod = kNHits) {
    switch (kMethod) {
      case kNHits:
        //printf("hits left: %g - hits right %g\n", fNHitsX[0], fNHitsX[1]);
        (fNHitsX[0] > fNHitsX[1]) ? fDirX = -1 : fDirX = +1;
        (fNHitsY[0] > fNHitsY[1]) ? fDirY = -1 : fDirY = +1;
        //printf("fDirX = %i\n", fDirX);
        break;
      case kCharge:
        (fChargeX[0] > fChargeX[1]) ? fDirX = -1 : fDirX = +1;
        (fChargeY[0] > fChargeY[1]) ? fDirY = -1 : fDirY = +1; 
        break;
      case kLength:
        (fLengthX[0] > fLengthX[1]) ? fDirX = -1 : fDirX = +1;
        (fLengthY[0] > fLengthY[1]) ? fDirY = -1 : fDirY = +1; 
        break;
    }
    return;
  }
};

class SLArQEventAnalysis : public TObject {
  public: 
    SLArQEventAnalysis(); 
    SLArQEventAnalysis(const SLArCfgAnode* anodeCfg); 
    ~SLArQEventAnalysis(); 

    cluster_projection_info_t ProcessProjection(const TString& proj, SLArQEventReadout* qevent, const TVector3& vtx); 
    void DrawProjection(SLArQEventReadout* qev, const TVector3& vtx, const cluster_projection_info_t& proj_info); 

  protected:
    const SLArCfgAnode* fCfgAnode; 
    TF1* fLine; 
    void read_and_fill_axis(const TString& axis_str, std::vector<TVector3>* axis_vec, std::vector<int>* axis_idx);
    int  scan_cluster_proj(const TH2* h2, const int ix_vtx, const int iy_vtx, cluster_projection_info_t& proj_info); 
    void init_line_pars(const TGraphErrors& g, const float* fit_range); 

  public: 
    ClassDef(slarq::SLArQEventAnalysis, 1)
};
}
#endif /* end of include guard SLARQEVENTANALYSIS_HH */

