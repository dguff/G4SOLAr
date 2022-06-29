/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : process_events.cpp
 * @created     : lunedì giu 27, 2022 13:54:58 CEST
 */

#include <iostream>
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
                                   

//***********************************************
// FUNCTIONS FORWARD DEFINITIONS
double view_event(SLArMCEvent* ev, bool do_draw = false);
double q_diff_and_record(SLArMCEvent* ev, slarq::SLArQReadout* qev); 
void   display(SLArMCEvent* ev, slarq::SLArQReadout* qev, TCanvas* cv); 


int analyze_file(const char* path, bool do_draw = false) {

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

  TCanvas* cTmp = new TCanvas("cTmp", "ev display", 0, 0, 800, 800); 
  cTmp->Divide(2, 2); 
  TTimer  *timer = new TTimer("gSystem->ProcessEvents();", 500, kFALSE);

  for (int iev = 0; iev<500; iev++) {
    if (iev%20 == 0) {
      printf("processing ev %i (%.2f%%)\n", iev,  100*iev / (double)t->GetEntries()); 
    }
    t->GetEntry(iev); 

    ev_q->SetEventNr(iev); 
    double te = 0.; 
    for (const auto &p : ev->GetPrimaries()) {
      te += p->GetTotalEdep(); 
    }

    double qtot = q_diff_and_record(ev, ev_q); 

    //printf("qtot = %g - expected = %g [%g MeV]\n", qtot, te / W, te);
    ev_q->Clustering(); 

    timer->TurnOn(); 
    timer->Reset(); 
    display(ev, ev_q, cTmp); 
    getchar(); 

    timer->TurnOff(); 

    qtree->Fill(); 

    ev_q->ResetEvent(); 

    printf("\n\n"); 
  }

  qtree->Write(); 
  file_out->Close(); 

  return 1.;
}

double q_diff_and_record(SLArMCEvent* ev, slarq::SLArQReadout* qev) {
  auto primaries = ev->GetPrimaries(); 
  double q_ev_obs = 0.;

  size_t ip = 0; 
  for (const auto &p : primaries) {
    p->PrintParticle(); 
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

double view_event(SLArMCEvent* ev, bool do_draw) 
{
  
  double edep_tot = 0.;
  double xmin = 1e8, xmax = -1e8, ymin = 1e8, ymax = -1e8, zmin = 1e8, zmax = -1e8; 

  auto primaries = ev->GetPrimaries(); 

  TGraph2D* gT[primaries.size()];
  for (size_t iip=0; iip<primaries.size(); iip++) gT[iip] = nullptr;
  size_t ip = 0; 
  for (const auto &p : primaries) {
    p->PrintParticle();
    auto trajectories = p->GetTrajectories(); 
    //printf("%lu associated trajectories\n", trajectories.size()); 
    int itrj = 0;
    if (trajectories.size() > 0) {
      double primary_edep = 0; 
      for (const auto &trj : trajectories) {
        //printf("trajectory %i has %lu points\n", itrj, trj->GetPoints().size()); 
        if (trj->GetPoints().size() > 0) {
          for (const auto &tp : trj->GetPoints()) {
            if (do_draw) {
              if (!gT[ip]) gT[ip] = new TGraph2D(); 
              gT[ip]->AddPoint(tp.fX, tp.fY, tp.fZ); 
              //printf("point at (%g, %g, %g) - %g MeV\n", 
                  //tp.fX, tp.fY, tp.fZ, tp.fEdep); 
              if (tp.fX > xmax) xmax = 1.1*tp.fX; 
              else if (tp.fX < xmin) xmin = tp.fX; 
              if (tp.fY > ymax) ymax = 1.1*tp.fY; 
              else if (tp.fY < ymin) ymin = tp.fY; 
              if (tp.fZ > zmax) zmax = 1.1*tp.fZ; 
              else if (tp.fZ < zmin) zmin = tp.fZ; 
            }
            primary_edep += tp.fEdep; 
          }

        }

        itrj++;
      }
      edep_tot += primary_edep;
      if (do_draw) {
        printf("--> Trajectory energy deposit = %g MeV -> %g MeV\n", primary_edep, edep_tot); 
      }
    }
    ip++;

  }

  TH3D* h3 = nullptr;
  if (do_draw) {
    h3 = new TH3D("frame", "frame", 30, xmin, xmax, 30, ymin, ymax, 30, zmin, zmax);
    h3->Draw("axis"); 
    for (size_t iip = 0; iip<primaries.size(); iip++) {
      if (!gT[iip]) {printf("graph for primary %lu not found\n", iip);}
      else gT[iip]->Draw("P0"); 
    }

    gPad->Modified(); 
    gPad->Update(); 
    getchar(); 

    gPad->Clear(); 
    delete h3; 
    for (size_t iip=0; iip<primaries.size(); iip++) {
      delete gT[iip];
    }
  }

  return edep_tot;
}

