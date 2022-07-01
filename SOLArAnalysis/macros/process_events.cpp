/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : process_events.cpp
 * @created     : lunedì giu 27, 2022 13:54:58 CEST
 */

#include <iostream>
#include "Math/GenVector/RotationZYX.h"
#include "Math/Point3Dfwd.h"
#include "Math/Vector3Dfwd.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TGraph2D.h"
#include "TTimer.h"

#include "event/SLArMCEvent.hh"
#include "event/SLArMCPrimaryInfo.hh"

#include "SLArQConstants.h"
#include "SLArQReadout.hh"
#include "SLArQDiffusion.hh"
#include "SLArQEvReco.hh"

// FUNCTIONS FORWARD DEFINITIONS
double view_event(SLArMCEvent* ev, bool do_draw = false);
double q_diff_and_record(SLArMCEvent* ev, slarq::SLArQReadout* qev); 
void   display(SLArMCEvent* ev, slarq::SLArQReadout* qev, TCanvas* cv); 


int analyze_file(const char* path, bool do_draw = false) {
  // Check if coordinate transformation is correctly defined
  printf("slarq::xshift: [%.2f, %.2f, %.2f] mm\n", 
      slarq::xshift[0], slarq::xshift[1], slarq::xshift[2]); 

  // Open and setup readout of the input file
  TFile* file_in = new TFile(path); 
  TTree* t = (TTree*)file_in->Get("EventTree"); 
  SLArMCEvent* ev = 0; 
  t->SetBranchAddress("MCEvent", &ev); 

  // Open and setup output file
  TString outfile_name = path; 
  outfile_name.Resize( outfile_name.Index(".root") ); 
  outfile_name += "_analysis.root"; 
  TFile* file_out = new TFile(outfile_name, "recreate"); 
  TTree* qtree = new TTree("qtree", "charge readout tree"); 

  slarq::SLArQReadout* ev_q = new slarq::SLArQReadout(0, "ev_q");
  ev_q->SetReadoutAxis(slarq::kTime, 0, 400, 1.874, "Time [#mus]"); 
  ev_q->SetReadoutAxis(slarq::kX, 0., 1.2e3, 3.0, "#it{x} [mm]"); 
  ev_q->SetReadoutAxis(slarq::kY, 0., 2.3e3, 3.0, "#it{y} [mm]"); 
  ev_q->SetReadoutAxis(slarq::kZ, 0., 6.0e3, 3.0, "#it{z} [mm]"); 

  ev_q->BuildHistograms();
  qtree->Branch("qreadout", &ev_q); 

  TCanvas* cTmp = new TCanvas("cTmp", "ev display", 0, 0, 1000, 700); 
  cTmp->Divide(3,2); 
  TTimer  *timer = new TTimer("gSystem->ProcessEvents();", 500, kFALSE);
  
  TH1D* cos_theta_hist = new TH1D("cos_theta_hist","cos of scattering angle",100,-1,1);
  TH1D* rot_cos_theta_hist = new TH1D("rot_cos_theta_hist","cos of scattering angle after pca",100,-1,1);


  for (int iev = 0; iev<50; iev++) {
    if (iev%20 == 0) {
      printf("processing ev %i (%.2f%%)\n", iev,  100*iev / (double)t->GetEntries()); 
    }
    t->GetEntry(iev); 

    ev_q->SetEventNr(iev); 
    double te = 0.; 
    double temax = 0; 
    std::vector<double> vertex(3, 0.); 
    for (const auto &p : ev->GetPrimaries()) {
      te += p->GetTotalEdep(); 
      if (p->GetTotalEdep() > temax) {
        temax = p->GetTotalEdep();
        auto pvertex = p->GetVertex(); 
        vertex[0] = pvertex.at(0) + slarq::xshift[0]; 
        vertex[1] = pvertex.at(1) + slarq::xshift[1];
        vertex[2] = pvertex.at(2) + slarq::xshift[2]; 
        //printf("setting vertex to [%.2f, %.2f, %.2f]\n", 
        //    pvertex.at(0), pvertex.at(1), pvertex.at(2)); 

        //printf("energy deposition = %f \n", temax);
      }
    }

    double qtot = q_diff_and_record(ev, ev_q); 

    //printf("vertex is (%.2f, %.2f, %.2f)\n", vertex[0], vertex[1], vertex[2]); 
    ev_q->Clustering(); 

    slarq::SLArQEvReco reco(ev_q); 
    reco.PCA(); 
    auto hn_cluster = ev_q->GetMaxClusterHn();
    reco.SetVertex(&vertex[0]); 

    reco.ClusterFit(hn_cluster);
    double cos_theta = reco.GetCosAngle( ROOT::Math::XYZVectorD(0, 0, 1) );
    
    slarq::SLArQBlipPCA* bpca = reco.GetPCA();
    auto rot = bpca->GetRotation();
    auto inverse_rot = rot.Inverse();
    auto rot_cluster = reco.RotateCluster(rot, ROOT::Math::XYZPointD(vertex[0],vertex[1],vertex[2]));
    reco.ClusterFit(rot_cluster);
    double rot_cos_theta = reco.GetCosAngle(ROOT::Math::XYZVectorD(0,0,1),&inverse_rot);
    
    printf("cos theta before rotation = %f and after rotation = %f \n", cos_theta,rot_cos_theta);
    cos_theta_hist->Fill(cos_theta);
    rot_cos_theta_hist->Fill(rot_cos_theta); 


    timer->TurnOn(); 
    timer->Reset(); 
    //display(ev, ev_q, cTmp); 
    cTmp->cd(1); 
    hn_cluster->Projection(1, 0)->Draw("col");
    cTmp->cd(2); 
    hn_cluster->Projection(2, 0)->Draw("col");
    cTmp->cd(3);
    hn_cluster->Projection(0, 1, 2)->Draw("box");
    cTmp->cd(4); 
    rot_cluster->Projection(1, 0)->Draw("col");
    cTmp->cd(5); 
    rot_cluster->Projection(2, 0)->Draw("col");
    cTmp->cd(6); 
    rot_cluster->Projection(0, 1, 2)->Draw("box");
    
    getchar(); 

    timer->TurnOff(); 

    qtree->Fill(); 

    ev_q->ResetEvent(); 
    delete hn_cluster; 

    printf("\\n"); 
  }

 // timer->TurnOn();
 // timer->Reset();
 // cTmp->cd(1);
 // timer->TurnOff();
  //getchar();

  qtree->Write(); 

  rot_cos_theta_hist->SetLineColor(kRed);
  rot_cos_theta_hist->Draw("hist");
  cos_theta_hist->Draw("hist same");

  cTmp->Modified();
  cTmp->Update();

  return 1.;
}

double q_diff_and_record(SLArMCEvent* ev, slarq::SLArQReadout* qev) {
  auto primaries = ev->GetPrimaries(); 
  double q_ev_obs = 0.;

  size_t ip = 0; 
  for (const auto &p : primaries) {
    //p->PrintParticle(); 
    double primary_edep = 0; 
    auto trajectories = p->GetTrajectories(); 
    printf("%lu associated trajectories\n", trajectories.size()); 
    int itrj = 0;

    slarq::SLArQDiffusion diff; 

    for (const auto &trj : trajectories) {
      //printf("trajectory %i has %lu points\n", itrj, trj->GetPoints().size()); 
      if (trj->GetPoints().size() > 0 && 
          !(trj->GetParticleName().Contains("nu"))) {
        for (const auto &tp : trj->GetPoints()) {
          double q = diff.DiffuseRandom(&tp,  qev); 
        }
        primary_edep += trj->GetTotalEdep(); 
      }

      itrj++;
    } // end of loop over trajectories
    q_ev_obs += primary_edep; 
    printf("total energy loss in trajectories: %g MeV\n", primary_edep); 

    ip++;

  } // end of loop over primary particles

  qev->ApplySuppressionAndQRec(100); 

  return q_ev_obs; 
}

void display(SLArMCEvent* ev, slarq::SLArQReadout* qev, TCanvas* cv) {
  // get readout hist
  TH2D* hqzx = (TH2D*)qev->GetHist(slarq::kZ); 
  TH2D* hqyx = (TH2D*)qev->GetHist(slarq::kY); 


  std::vector<TGraph*> gTz; 
  std::vector<TGraph*> gTy; 
  auto primaries = ev->GetPrimaries(); 
  for (const auto &p : primaries) {
    auto trajectories = p->GetTrajectories(); 
    int itj = 0; 
    for (const auto &t : trajectories) {
      TString p_name = t->GetParticleName(); 
      if (!p_name.Contains("nu")) {
        TGraph* gz = new TGraph(t->GetPoints().size()); 
        TGraph* gy = new TGraph(t->GetPoints().size()); 
        size_t ipt = 0; 
        for (const auto &point : t->GetPoints()) {
          gz->SetPoint(ipt, point.fZ+slarq::xshift[2], 
              (point.fX+slarq::xshift[0]) / Vdrift); 
          gy->SetPoint(ipt, point.fY+slarq::xshift[1], 
              (point.fX+slarq::xshift[0]) / Vdrift); 
          ipt++;
        }
        gz->SetName(Form("gz_ev%i_%s_t%i", ev->GetEvNumber(), t->GetParticleName().Data(), itj)); 
        gy->SetName(Form("gy_ev%i_%s_t%i", ev->GetEvNumber(), t->GetParticleName().Data(), itj)); 
        gTz.push_back(gz); 
        gTy.push_back(gy);
        itj++; 
      }
    }
  }

  cv->cd(2); 
  hqzx->Draw("col");
  for (auto &g : gTz) g->Draw("l same"); 

  cv->cd(3); 
  hqyx->Draw("col"); 
  for (auto &g : gTy) g->Draw("l same"); 

  for (const auto &cluster : qev->GetClusters()) {
    printf("cluster %lu: Q = %g,  %lu points\n", 
        cluster->get_id(), cluster->get_charge(), cluster->get_points().size()); 
  }

  auto hclusters = qev->GetClusterHists(); 
  int ih = 0; 
  cv->cd(4); 
  for (const auto &h3 : hclusters) {
    if (ih == 0 ) h3->Draw("box"); 
    else h3->Draw("box same"); 
    ih++; 
  }

  cv->Modified(); 
  cv->Update(); 
  getchar(); 

  for (auto &h3 : hclusters) delete h3; 
  hclusters.clear(); 
  for (auto &g : gTy) delete g; 
  for (auto &g : gTz) delete g; 
}

int main(int argc, const char* argv[]) {
  
  if (argc == 1) {
    printf("please specify input file path\n"); 
    return 0; 
  }

  double q = analyze_file(argv[1], false); 

  return 1; 
}


