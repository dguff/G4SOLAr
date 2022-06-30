/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQReadout
 * @created     : lunedì giu 27, 2022 13:20:12 CEST
 */

#include "SLArQReadout.hh"
#include "SLArQConstants.h"

#include "TStyle.h"
#include "TRandom3.h"

ClassImp(slarq::SLArQReadout)

namespace slarq {

  slarq_axis::slarq_axis(TString titl, int nb, double xmin, double xmax, double pitch) {
    fTitle = titl; 
    fNbins = nb;
    fXmin = xmin;
    fXmax = xmax;
    fPitch = pitch;
  }


  SLArQReadout::SLArQReadout() 
    : TNamed(), fIEv(-1), fHTx(0), fHQyt(0), fHQzt(0), fHQn(0), 
    fAxis(4, slarq_axis())
  {
    fClusters.reserve(20); 
  }

  SLArQReadout::SLArQReadout(int iev, const TString name, const TString titl) 
    : TNamed(name, titl), fIEv(iev), fHTx(0), fHQyt(0), fHQzt(0), fHQn(0),
    fAxis(4, slarq_axis())
  {
    fClusters.reserve(20); 
  }

  SLArQReadout::SLArQReadout(const SLArQReadout& qout) 
    : TNamed(qout), fIEv(qout.fIEv), fHTx(0), fHQyt(0), fHQzt(0), fHQn(0), 
    fAxis(4, slarq_axis())
  {
    if (fHTx ) {delete fHTx;}
    if (fHQyt) {delete fHQyt;}
    if (fHQzt) {delete fHQzt;}
    if (fHQn ) {delete fHQn;}

    for (auto &cl : fClusters) {
      delete cl; cl = nullptr; 
    }
    fClusters.clear(); 

    if (qout.fHTx)
      fHTx = (TH1D*)qout.fHTx->Clone(); 
    if (qout.fHQyt)
      fHQyt = (TH2D*)qout.fHQyt->Clone(); 
    if (qout.fHQzt)
      fHQzt = (TH2D*)qout.fHQzt->Clone(); 
    if (qout.fHQn) 
      fHQn = (THnSparseD*)qout.fHQn->Clone(); 

    fClusters = qout.fClusters;

    return;
  }

  SLArQReadout::~SLArQReadout()
  {
    if (fHTx ) {delete fHTx;}
    if (fHQyt) {delete fHQyt;}
    if (fHQzt) {delete fHQzt;}
    if (fHQn ) {delete fHQn;}

    for (auto &cl : fClusters) {
      delete cl; cl = nullptr; 
    }
    fClusters.clear(); 
  }

  SLArQCluster* SLArQReadout::GetMaxCluster() {
    size_t iclmax = 0 ; 
    double q_tmp  = 0.;

    int icl = 0;
    for (auto &cl : fClusters) {
      if (cl->get_charge() > q_tmp) {
        iclmax = icl; 
        q_tmp = cl->get_charge(); 
      }
      icl++;
    }

    return fClusters.at(iclmax); 
  }

  TH3D* SLArQReadout::GetMaxClusterHist() {
    TH3D* h = fHQn->Projection(0, 1, 2); 
    h->Reset(); 
    auto max_cluster = GetMaxCluster(); 
    h->SetName(Form("ev_%i_cluster_%lu_h3", fIEv, max_cluster->get_id())); 
    max_cluster->set_cluster_hist(h); 
    return h; 
  }

  THnSparseD* SLArQReadout::GetMaxClusterHn() {
    THnSparseD* hn_cluster = (THnSparseD*)fHQn->Clone("hn_cluster"); 
    hn_cluster->Reset(); 
    auto max_cluster = GetMaxCluster(); 
    hn_cluster->SetName(Form("ev_%i_cluster_%lu_hn", fIEv, max_cluster->get_id())); 
    max_cluster->set_cluster_hist(hn_cluster); 
    return hn_cluster; 
  }

  std::vector<TH3D*> SLArQReadout::GetClusterHists() {
    gStyle->SetPalette(kRainBow); 
    const size_t ncols = 10; 
    Color_t cols[ncols]; 
    for (size_t icol = 0; icol <ncols; icol++) {
      cols[icol] = gStyle->GetColorPalette(icol *255 / ncols); 
    }

    gStyle->SetPalette(kSunset); 


    std::vector<TH3D*> hcluster(fClusters.size(), nullptr); 
    size_t icl = 0; 
    for (const auto &cluster : fClusters) {
      hcluster[icl] = fHQn->Projection(0, 1, 2); 
      hcluster[icl]->Reset(); 
      hcluster[icl]->SetName(Form("ev_%i_cluster_%lu_h3", fIEv, cluster->get_id())); 
      cluster->set_cluster_hist(hcluster[icl]);
      hcluster[icl]->SetLineWidth(2); 
      if (icl < ncols) {
        hcluster[icl]->SetFillColor(cols[icl]);
      } else {
        hcluster[icl]->SetFillColor(cols[ncols-1]); 
      }
      hcluster[icl]->SetLineColor(kBlack); 

      icl++;
    }

    return hcluster;
  }

  TH1* SLArQReadout::GetHist(EAxis kAxis) {
    if      (kAxis == slarq::kTime) return  fHTx; 
    else if (kAxis == slarq::kY   ) return  fHQyt; 
    else if (kAxis == slarq::kZ   ) return  fHQzt; 

    return nullptr;
  }

  double SLArQReadout::GetQReadoutPitchAxis(EAxis kAxis) {
    double pitch = 0.;
    if      (kAxis == slarq::kTime) pitch = fAxis[kTime].fPitch;
    else if (kAxis == slarq::kY)    pitch = fAxis[kY].fPitch;
    else if (kAxis == slarq::kZ)    pitch = fAxis[kZ].fPitch; 
    else if (kAxis == slarq::kX)    pitch = fAxis[kX].fPitch; 

    return pitch;

  }

  double SLArQReadout::GetQReadoutXminAxis(EAxis kAxis) {
    double xmin = 0.;
    if      (kAxis == slarq::kTime) xmin = fAxis[kTime].fXmin;
    else if (kAxis == slarq::kY   ) xmin = fAxis[kY].fXmin;
    else if (kAxis == slarq::kZ   ) xmin = fAxis[kZ].fXmin;
    else if (kAxis == slarq::kX   ) xmin = fAxis[kX].fXmin;

    return xmin;

  }

  double SLArQReadout::GetQReadoutXmaxAxis(EAxis kAxis) {
    double xmax = 0.;
    if      (kAxis == slarq::kTime) xmax = fAxis[kTime].fXmax;
    else if (kAxis == slarq::kY   ) xmax = fAxis[kY].fXmax;
    else if (kAxis == slarq::kZ   ) xmax = fAxis[kZ].fXmax;
    else if (kAxis == slarq::kX   ) xmax = fAxis[kX].fXmax;

    return xmax;

  }

  void SLArQReadout::SetReadoutAxis(EAxis kAxis, double xmin, double xmax, 
      double pitch, TString title) {

    int N = (xmax - xmin) / pitch; 
    fAxis[kAxis].fXmin = xmin; 
    fAxis[kAxis].fXmax = xmin + N*pitch;
    fAxis[kAxis].fNbins = N; 
    fAxis[kAxis].fTitle = title; 
    fAxis[kAxis].fPitch = pitch; 

    return;
  }

  void SLArQReadout::BuildHistograms() {
    if (fHTx) delete fHTx;
    TString hname = Form("htx_ev_%i", fIEv); 
    TString htitl = Form("%s;%s;Entries", hname.Data(), fAxis[kTime].fTitle.Data());
    fHTx = new TH1D(hname, htitl, fAxis[kTime].fNbins, 
        fAxis[kTime].fXmin, fAxis[kTime].fXmax); 

    if (fHQyt) delete fHQyt; 
    hname = Form("hq_yt_ev_%i", fIEv); 
    htitl = Form("%s;%s;%s", hname.Data(), 
        fAxis[kY].fTitle.Data(), fAxis[kTime].fTitle.Data());
    fHQyt = new TH2D(hname, htitl, 
        fAxis[kY].fNbins, fAxis[kY].fXmin, fAxis[kY].fXmax, 
        fAxis[kTime].fNbins, fAxis[kTime].fXmin, fAxis[kTime].fXmax); 

    if (fHQzt) delete fHQzt; 
    hname = Form("hq_zt_ev_%i", fIEv); 
    htitl = Form("%s;%s;%s", hname.Data(), 
        fAxis[kZ].fTitle.Data(), fAxis[kTime].fTitle.Data());
    fHQzt = new TH2D(hname, htitl, 
        fAxis[kZ].fNbins, fAxis[kZ].fXmin, fAxis[kZ].fXmax, 
        fAxis[kTime].fNbins, fAxis[kTime].fXmin, fAxis[kTime].fXmax); 

    if (fHQn) delete fHQn; 
    hname = Form("hq_xyz_ev_%i", fIEv); 
    htitl = Form("%s;%s;%s;%s", hname.Data(), 
        fAxis[kX].fTitle.Data(), fAxis[kY].fTitle.Data(), fAxis[kZ].fTitle.Data());
    int bin_[3]; double xmin_[3]; double xmax_[3]; 

    bin_ [0] = fAxis[kX].fNbins; 
    xmin_[0] = fAxis[kX].fXmin;  
    xmax_[0] = fAxis[kX].fXmax;  

    bin_ [1] = fAxis[kY].fNbins;   
    xmin_[1] = fAxis[kY].fXmin;    
    xmax_[1] = fAxis[kY].fXmax;    

    bin_ [2] = fAxis[kZ].fNbins; 
    xmin_[2] = fAxis[kZ].fXmin; 
    xmax_[2] = fAxis[kZ].fXmax; 

    fHQn = new THnSparseD(hname, htitl, 3, bin_, xmin_, xmax_);  
  }

  void SLArQReadout::ResetEvent() {
    for (auto cl : fClusters) {
      delete cl; cl = nullptr; 
    }
    fClusters.clear(); 

    if (fHTx ) fHTx ->Reset(); 
    if (fHQyt) fHQyt->Reset(); 
    if (fHQzt) fHQzt->Reset(); 
    if (fHQn ) {
      for (int i=0; i<3; i++) {
        fHQn->GetAxis(i)->SetRange(); 
      }
      fHQn ->Reset(); 
    }

    return;
  }

  double SLArQReadout::GetTotalCharge() {
    double qtot = 0.; 
    for (const auto &cl : fClusters) qtot += cl->get_charge(); 

    return qtot;
  }

  void SLArQReadout::Record(double t, double* xx, double w) {
    fHTx->Fill(t);
    fHQyt->Fill(xx[1], t, w); 
    fHQzt->Fill(xx[2], t, w); 
    fHQn->Fill(xx, w); 
  }

  void SLArQReadout::ApplySuppressionAndQRec(double thrs) {

    double bc = 0.; 

    for (int ix =1; ix < fHQyt->GetNbinsX(); ix++) {
      for (int iy =1; iy < fHQyt->GetNbinsY(); iy++) {
        bc = fHQyt->GetBinContent(ix, iy); 
        if (bc < thrs) {
          fHQyt->SetBinContent(ix, iy, 0.); 
        } else {
          double t_ = fHQyt->GetYaxis()->GetBinCenter(iy); 
          fHQyt->SetBinContent(ix, iy, bc * exp( t_ / Elifetime ));
        }
      }
    }

    for (int ix =1; ix < fHQzt->GetNbinsX(); ix++) {
      for (int iy =1; iy < fHQzt->GetNbinsY(); iy++) {
        bc = fHQzt->GetBinContent(ix, iy);
        if (bc < thrs) {
          fHQzt->SetBinContent(ix, iy, 0.); 
        } else {
          double t_ = fHQzt->GetYaxis()->GetBinCenter(iy); 
          fHQzt->SetBinContent(ix, iy, bc * exp( t_ / Elifetime ));
        }
      }
    }

    auto it = fHQn->CreateIter(false); 
    int ibin_[3] = {0};
    Long64_t ibin = 0; 
    while ( (ibin = it->Next()) >= 0) { 
      bc = fHQn->GetBinContent(ibin, ibin_);
      if (bc < thrs) {
        //printf("setting bin content to 0..."); 
        fHQn->SetBinContent(ibin, 0.); 
        //printf(" DONE\n");
      }
      else {
        double t_ = fHQn->GetAxis(0)->GetBinCenter(ibin_[0]) / Vdrift; 
        //printf("correcting charge for recombination effect (t = %.2f μs)...", 
        //t_); 
        fHQn->SetBinContent(ibin, bc * exp( t_ / Elifetime )); 
        //printf(" DONE\n"); 
      }
    }

    return;
  }

  size_t SLArQReadout::Clustering() {

    adjust_hn_range();

    THnSparseD* maincluster = (THnSparseD*)fHQn->Clone("main_cluster");
    const int h_rank = maincluster->GetNdimensions(); 
    std::vector<int> idx(h_rank, 0); 
    std::vector<int> idx_(h_rank, 0.); 
    std::vector<double> xbin(h_rank, 0.); 
    std::vector<double> bw(h_rank, 0.);
    for (int k=0; k<h_rank; k++) {
      bw[k] = maincluster->GetAxis(k)->GetBinWidth(10); 
    } 

    auto iti = maincluster->CreateIter(false);
    Long64_t b =0;
    int n_max_trials = 5;

    while((b=iti->Next()) >=0 ){
      double q = maincluster->GetBinContent(b, &idx[0]); 
      double q_= 0.; 

      bool bin_found = true; 
      int n_trial = 0; 

      while (bin_found || n_trial < n_max_trials) {

        bin_found = false;
        size_t cluster_id = 0;

        for (int idim=0; idim<3; idim++) {
          idx_ = idx; // set temporary idexes to current bin

          for (int delta_ = -2; delta_ <3; delta_++) {
            // scan the neighborhood of the current bin along the idim-axis
            idx_[idim] = idx[idim]+delta_; 
            q_ = maincluster->GetBinContent(&idx_[0]); 
            if (q_ > 10) {
              cluster_point point; 
              point.fBin = maincluster->GetBin(&idx_[0]); 
              for (int jdim=0; jdim<3; jdim++) {
                xbin[jdim] = maincluster->GetAxis(jdim)->GetBinCenter(idx_[jdim]);
              }
              point.fPos.SetXYZ(xbin[0], xbin[1], xbin[2]); 
              point.fCharge = q_;

              cluster_id = find_cluster(&point);

              // set the bin content to zero to prevent double-counting
              maincluster->SetBinContent(point.fBin, 0.); 

              bin_found = true;
            }
          }
        }

        //printf("fClusters size = %lu - cluster id = %lu\n", fClusters.size(), cluster_id); 
        double n_points = fClusters[cluster_id]->get_points().size(); 
        auto item = fClusters[cluster_id]->get_points().begin();
        if (gRandom->Rndm() > 0.5) {
          // look around the HEAD
          int p_rndm = TMath::Min(5*gRandom->Rndm(), n_points-1); 
          //printf("look at cluster element %i [size = %g]\n", p_rndm, n_points); 
          std::advance( item, p_rndm);
        } else {
          // look around the TAIL
          int p_rndm = TMath::Max(0., n_points - 5*gRandom->Rndm()-1);
          //printf("look at cluster element %i [size = %g]\n", p_rndm, n_points); 
          std::advance( item, p_rndm ); 
        }

        //std::cout << "switch to bin " << item->fBin << std::endl;
        maincluster->GetBinContent(item->fBin, &idx[0]); 
        n_trial++;
        //printf("bin_found = %i, n_trial = %i\n", (int)bin_found, n_trial); 
      }

    }

    delete maincluster; 
    return fClusters.size(); 
  }

  size_t SLArQReadout::find_cluster(cluster_point* point) {
    const double dmax = 15; 

    if (fClusters.size() == 0) {
      SLArQCluster* clstr = new SLArQCluster(0); 
      clstr->register_point(*point); 
      //printf("creating new cluster: adding bin %i to cluster %lu[%lu]\n", 
          //(int)point->fBin, clstr->get_id(), clstr->get_points().size()); 
      fClusters.push_back(clstr);
      return clstr->get_id();
    } else {
      for (auto clstr : fClusters) {
        if ( clstr->is_registered(point->fBin) ) {
          //printf("bin %i is already registered in cluster %lu\n", 
              //(int)point->fBin, clstr->get_id()); 
          return 666;
        }
        for (const auto &clstr_point : clstr->get_points()) {
          double dist2 = (point->fPos - clstr_point.fPos).Mag2();
          if (dist2 <= dmax*dmax) {
            //printf("adding bin %i to cluster %lu[%lu]\n", 
                //(int)point->fBin, clstr->get_id(), clstr->get_points().size()); 
            clstr->register_point(*point);
            return clstr->get_id();
          }
        }
      }
      SLArQCluster* clstr = new SLArQCluster( fClusters.back()->get_id()+1 ); 
      clstr->register_point(*point); 
      //printf("creating new cluster %lu for bin %i\n", 
          //clstr->get_id(), (int)point->fBin ); 

      fClusters.push_back(clstr);

      return clstr->get_id();
    }
  }

  void SLArQReadout::adjust_hn_range() {
    int xmin[3]; int xmax[3]; 
    for (int ik=0; ik<3; ik++) {
      TH1D* h_tmp = fHQn->Projection(ik);
      xmin[ik] = h_tmp->GetXaxis()->GetBinLowEdge(h_tmp->FindFirstBinAbove(10));
      xmax[ik] = h_tmp->GetXaxis()->GetBinUpEdge (h_tmp->FindLastBinAbove(10));

      fHQn->GetAxis(ik)->SetRangeUser(xmin[ik]-1,xmax[ik]+1);
      delete h_tmp;
    }
   
    return;
  }


}