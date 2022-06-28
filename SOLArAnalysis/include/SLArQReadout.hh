/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQReadout
 * @created     : lunedì giu 27, 2022 13:14:28 CEST
 */

#ifndef SLARQREADOUT_HH

#define SLARQREADOUT_HH

#include <iostream>
#include <stdio.h>

#include "TH1D.h"
#include "TH2D.h"
#include "THnSparse.h"

#include "SLArQCluster.hh"

namespace slarq {

  enum EAxis {kTime = 0, kY = 1, kZ = 2, kX = 3}; 

  struct slarq_axis {
    TString fTitle; 
    double  fXmin; 
    double  fXmax;
    double  fPitch;
    int     fNbins;

    slarq_axis() {}
    slarq_axis(TString, int, double, double, double); 
  };

  class SLArQReadout : public TNamed 
  {
    public: 
      SLArQReadout(); 
      SLArQReadout(int iev, const TString, const TString = ""); 
      SLArQReadout(const SLArQReadout& qout); 
      ~SLArQReadout(); 


      void   ApplySuppressionAndQRec(double thrs); 
      void   BuildHistograms();

      size_t Clustering(); 

      double GetQReadoutXminAxis (EAxis kAxis); 
      double GetQReadoutXmaxAxis (EAxis kAxis); 
      double GetQReadoutPitchAxis(EAxis kAxis); 

      double GetTotalCharge(); 
      std::vector<SLArQCluster*>& GetClusters() {return fClusters;}
      SLArQCluster* GetMaxClusters(); 
      TH1* GetHist(EAxis kAxis);
      THnSparseD* GetQHistN() {return fHQn;}
      int GetEventNr() {return fIEv;}

      void Record(double t, double* x, double w=1); 
      void ResetEvent();

      void SetEventNr(int iev) {fIEv = iev;}
      void SetReadoutAxis(EAxis kAxis, double xmin, double xmax, 
          double pitch, TString titl = "");

    private: 
      size_t find_cluster(cluster_point* point); 

      int   fIEv;
      TH1D* fHTx; 
      TH2D* fHQyt; 
      TH2D* fHQzt;

      THnSparseD* fHQn; 

      std::vector<slarq_axis>    fAxis;
      std::vector<SLArQCluster*> fClusters;

    public: 
      ClassDef(slarq::SLArQReadout, 1)
  };

}

#endif /* end of include guard SLARQREADOUT_HH */

