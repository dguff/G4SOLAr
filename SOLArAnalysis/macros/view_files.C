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

#include "event/SLArMCEvent.hh"
#include "event/SLArMCPrimaryInfo.hh"

#include "SLArQConstants.h"
#include "SLArQReadout.hh"
#include "SLArQDiffusion.hh"
                                   

//***********************************************
// FUNCTIONS FORWARD DEFINITIONS

int analyze_file(const char* path, bool do_draw = false) {

  // Open and setup readout of the input file
  TFile* file_in = new TFile(path); 
  TTree* t = (TTree*)file_in->Get("EventTree"); 
  SLArMCEvent* ev = 0; 
  t->SetBranchAddress("MCEvent", &ev); 

  TH2D* hxy = new TH2D("hxy", "X-Y projection", 50, -500, +500, 50, -2000, +2000); 
  TH2D* hyz = new TH2D("hyz", "Y-Z projection", 50, -2000, +2000, 50, -3000, +3000); 
  TH2D* hxz = new TH2D("hxz", "X-Z projection", 50, -500, +500, 50, -3000, +3000); 

  // Open and setup output file
  for (int iev = 0; iev<t->GetEntries(); iev++) {
    t->GetEntry(iev); 

    auto primaries = ev->GetPrimaries(); 
    double q_ev_obs = 0.;

    size_t ip = 0; 
    for (const auto &p : primaries) {
      double primary_edep = 0; 
      auto trajectories = p->GetTrajectories(); 
      //printf("%lu associated trajectories\n", trajectories.size()); 
      int itrj = 0;

      if (trajectories.size() > 0) {
        for (const auto &trj : trajectories) {
          //printf("trajectory %i has %lu points\n", itrj, trj->GetPoints().size()); 
          if (trj->GetPoints().size() > 0) {
            for (const auto &tp : trj->GetPoints()) {
              hxy->Fill(tp.fX, tp.fY, tp.fEdep); 
              hxz->Fill(tp.fX, tp.fZ, tp.fEdep); 
              hyz->Fill(tp.fY, tp.fZ, tp.fEdep); 
            }
          }
          itrj++;
        } // end of loop over trajectories
      }
      ip++;
    }
  }


  TCanvas* cProjections = new TCanvas("cProjections", "cProjections", 0, 0, 1000, 600); 
  cProjections->Divide(3, 1); 
  
  cProjections->cd(1); 
  hxy->Draw("col"); 

  cProjections->cd(2); 
  hxz->Draw("col"); 

  cProjections->cd(3); 
  hyz->Draw("col"); 
  return hyz->GetEntries(); 
}

void view_q_hist(const char* path) {
  TFile* file = new TFile(path); 
  TTree* qt = (TTree*)file->Get("qtree"); 
  slarq::SLArQReadout* qev = 0; 
  qt->SetBranchAddress("qreadout", &qev); 

  TH1D* hq = new TH1D("hq", "hq", 1000, 0, 10000); 

  for (int iev = 0; iev < qt->GetEntries(); iev++) {
    qt->GetEntry(iev); 
    auto hn = qev->GetQHistN(); 

    Long64_t ibin = 0; 
    auto it = hn->CreateIter(false); 
    while ( (ibin=it->Next()) >= 0) {
      double q = hn->GetBinContent(ibin); 
      hq->Fill(q); 
    }
  }

  hq->Draw("hist"); 
}

