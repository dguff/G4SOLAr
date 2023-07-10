/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQEventReadout.hh
 * @created     : Mon Jun 27, 2022 13:14:28 CEST
 */

#ifndef SLArQEventReadout_HH

#define SLArQEventReadout_HH

#include <iostream>
#include <stdio.h>

#include "TH1D.h"
#include "TH2D.h"
#include "THnSparse.h"

#include "SLArQCluster.hh"

namespace slarq {

  enum EAxis {kX = 0, kY = 1, kZ = 2, kTime = 3}; 
  extern TString AxisLabel[4]; 

  struct slarq_axis {
    TString fTitle; 
    double  fXmin; 
    double  fXmax;
    double  fPitch;
    int     fNbins;

    slarq_axis() {}
    slarq_axis(TString, int, double, double, double); 
  };

  class SLArQEventReadout : public TNamed 
  {
    public: 
      SLArQEventReadout(); 
      SLArQEventReadout(int iev, const TString, const TString = ""); 
      SLArQEventReadout(const SLArQEventReadout& qout); 
      ~SLArQEventReadout(); 


      void   ApplySuppressionAndQRec(double thrs); 
      //void   BuildHistograms();
      void   SourceHits3DHist(THnSparseF* hits3d); 

      size_t Clustering(); 

      double GetQReadoutXminAxis (EAxis kAxis); 
      double GetQReadoutXmaxAxis (EAxis kAxis); 
      double GetQReadoutPitchAxis(EAxis kAxis); 

      double GetTotalCharge(); 
      std::vector<SLArQCluster*>& GetClusters() {return fClusters;}
      std::vector<TH3D*> GetClusterHists(); 
      SLArQCluster* GetMaxCluster(); 
      TH3D* GetMaxClusterHist();
      THnSparseF* GetMaxClusterHn(); 
      TH1* GetHist(EAxis kAxis);
      THnSparseF* GetQHistN() {return fHQn;}
      int GetEventNr() {return fIEv;}

      //void Record(double t, double* x, double w=1); 
      void ResetEvent();

      void SetEventNr(int iev) {fIEv = iev;}
      void SetReadoutAxis(EAxis kAxis, double xmin, double xmax, 
          double pitch, TString titl = "");

    private: 
      void   adjust_hn_range(); 
      size_t find_cluster(cluster_point* point); 

      int   fIEv;

      THnSparseF* fHQn; 

      std::vector<slarq_axis>    fAxis;
      std::vector<SLArQCluster*> fClusters;

    public: 
      ClassDef(slarq::SLArQEventReadout, 1)
  };

}

#endif /* end of include guard SLArQEventReadout_HH */

