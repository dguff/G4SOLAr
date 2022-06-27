/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQReadout
 * @created     : lunedì giu 27, 2022 13:20:12 CEST
 */

#include "SLArQReadout.hh"
#include "SLArQConstants.h"

ClassImp(slarq::SLArQReadout)

namespace slarq {

  slarq_axis::slarq_axis(TString titl, int nb, double xmin, double xmax) {
    fTitle = titl; 
    fNbins = nb;
    fXmin = xmin;
    fXmax = xmax;
  }


  SLArQReadout::SLArQReadout() 
    : TNamed(), fIEv(-1), fHTx(0), fHQyt(0), fHQzt(0), fHQn(0), 
    fAxis(4, slarq_axis())
    {}

  SLArQReadout::SLArQReadout(int iev, const TString name, const TString titl) 
    : TNamed(name, titl), fIEv(iev), fHTx(0), fHQyt(0), fHQzt(0), fHQn(0),
    fAxis(4, slarq_axis())
  {}

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

  SLArQCluster* SLArQReadout::GetMaxClusters() {
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

    return fClusters.at(icl); 
  }

  TH1* SLArQReadout::GetHist(EAxis kAxis) {
    TH1* h = nullptr; 
    if      (kAxis == kTime) h = fHTx; 
    else if (kAxis == kY) h = fHQyt; 
    else if (kAxis == kZ) h = fHQzt; 

    return h;
  }

  double SLArQReadout::GetQReadoutPitchAxis(EAxis kAxis) {
    double pitch = 0.;
    if      (kAxis == kTime) pitch = fHTx->GetBinWidth(1); 
    else if (kAxis == kY) pitch = fHQyt->GetXaxis()->GetBinWidth(1); 
    else if (kAxis == kZ) pitch = fHQzt->GetXaxis()->GetBinWidth(1); 

    return pitch;

  }

  double SLArQReadout::GetQReadoutXminAxis(EAxis kAxis) {
    double xmin = 0.;
    if      (kAxis == 0) xmin = fHTx->GetXaxis()->GetXmin(); 
    else if (kAxis == 1) xmin = fHQyt->GetXaxis()->GetXmin(); 
    else if (kAxis == 2) xmin = fHQzt->GetXaxis()->GetXmin(); 

    return xmin;

  }

  double SLArQReadout::GetQReadoutXmaxAxis(EAxis kAxis) {
    double xmax = 0.;
    if      (kAxis == 0) xmax = fHTx->GetXaxis()->GetXmax(); 
    else if (kAxis == 1) xmax = fHQyt->GetXaxis()->GetXmax(); 
    else if (kAxis == 2) xmax = fHQzt->GetXaxis()->GetXmax(); 

    return xmax;

  }

  void SLArQReadout::SetReadoutAxis(EAxis kAxis, double xmin, double xmax, 
      double pitch, TString title) {
   
    int N = (xmax - xmin) / pitch; 
    fAxis[kAxis].fXmin = xmin; 
    fAxis[kAxis].fXmax = xmin + N*pitch;
    fAxis[kAxis].fNbins = N; 
    fAxis[kAxis].fTitle = title; 

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
    for (int ii=0; ii<3; ii++) {
      bin_[ii] = fAxis[ii].fNbins; 
      xmin_[ii] = fAxis[ii].fXmin; 
      xmax_[ii] = fAxis[ii].fXmax; 
    }
    fHQn = new THnSparseD(hname, htitl, 3, bin_, xmin_, xmax_);  
  }

  void SLArQReadout::ResetEvent() {
    for (auto cl : fClusters) {
      delete cl; cl = nullptr; 
    }
    fClusters.clear(); 

    if (fHTx ) fHTx->Reset(); 
    if (fHQyt) fHQyt->Reset(); 
    if (fHQzt) fHQzt->Reset(); 
    if (fHQn ) fHQn->Reset(); 

    return;
  }

  double SLArQReadout::GetTotalCharge() {
    double qtot = 0.; 
    for (const auto &cl : fClusters) qtot += cl->get_charge(); 

    return qtot;
  }

  void SLArQReadout::Record(double t, double* xx) {
    fHTx->Fill(t);
    fHQyt->Fill(xx[1], t); 
    fHQzt->Fill(xx[2], t); 
    fHQn->Fill(xx); 
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
    while ( (ibin = it->Next()) != 0) { 
      bc = fHQn->GetBinContent(ibin, ibin_);
      if (bc < thrs) 
        fHQn->SetBinContent(ibin, 0.); 
      else {
        double t_ = fHQn->GetAxis(0)->GetBinCenter(ibin_[0]) / Vdrift; 
        fHQn->SetBinContent(ibin, bc * exp( t_ / Elifetime )); 
      }
    }

    return;
  }

}
