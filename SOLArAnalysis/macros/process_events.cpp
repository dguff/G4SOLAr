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
                                   

//***********************************************
// FUNCTIONS FORWARD DEFINITIONS
double view_event(SLArMCEvent* ev, bool do_draw = false);
double q_diff_and_record(SLArMCEvent* ev, slarq::SLArQReadout* qev); 

float Ldiffusion(float l) {
  double t = l / Vdrift; //μs
  float Ldiff = sqrt(2 * Dlong * t ); //mm
  return Ldiff;
}

float Tdiffusion(float l){
  double t = l / Vdrift; //s
  float Tdiff = sqrt(2 * Dtrns * t ); //mm
  return Tdiff;
}

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
  ev_q->SetReadoutAxis(slarq::kTime, 0, 800, 0.2, "Time [#mus]"); 
  ev_q->SetReadoutAxis(slarq::kX, 0., 1.2e3, 3.0, "#it{x} [mm]"); 
  ev_q->SetReadoutAxis(slarq::kY, 0., 2.3e3, 3.0, "#it{y} [mm]"); 
  ev_q->SetReadoutAxis(slarq::kZ, 0., 6.0e3, 3.0, "#it{z} [mm]"); 

  ev_q->BuildHistograms();
  qtree->Branch("qreadout", &ev_q); 

  TH1D* h1 = new TH1D("h1", "Energy deposit;Energy [MeV];Entries", 500, 0, 15); 

  for (int iev = 0; iev<t->GetEntries(); iev++) {
    if (iev%100 == 0) {
      printf("processing ev %i (%.2f%%)\n", iev,  100*iev / (double)t->GetEntries()); 
    }
    t->GetEntry(iev); 

    ev_q->SetEventNr(iev); 

    double qtot = q_diff_and_record(ev, ev_q); 

    h1->Fill(qtot); 

    ev_q->ResetEvent(); 
  }


  TCanvas* cEne = new TCanvas(); 
  h1->Draw("hist"); 

  return h1->GetEntries();
}

double q_diff_and_record(SLArMCEvent* ev, slarq::SLArQReadout* qev) {
  // ----------------------------------------------------
  // Variables definitions: (q = nr of electrons)
  double q_ev_true = 0.; //!< "true" nr of e created
  double q_ev_obs  = 0.; //!< nr of electrons detected
  double q_ev_rec  = 0.; //!< nr of electrons reconstructed (after recombination corr)

  double q_step_true = 0.; //!< nr of electrons produced in the step
  double q_step_true_exp = 0.; //!< expected nr of electrons in the step
  double q_step_obs  = 0.; //!< nr of electrons detected in the step
  double q_step_obs_exp = 0.;  //!< expected value of electrons detected in the step
  double q_step_rec  = 0.; //!< nr of electrons reconstructed (after recombination corr) 

  double xx[3] = {0};

  auto primaries = ev->GetPrimaries(); 

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
            double t = (tp.fX / Vdrift); //μs           

            q_step_true_exp = (tp.fEdep / W);
            q_step_true     = (int)gRandom->Poisson(q_step_true_exp); 
            q_step_obs_exp = q_step_true * exp(-t / Elifetime);
            q_step_obs = (int)gRandom->Poisson(q_step_obs_exp);

            double reco_weight = exp( t / Elifetime ); 

            for (int iq = 0; iq<q_step_obs; iq++) {
              xx[0] = t * Vdrift + gRandom->Gaus(0., Ldiffusion(tp.fX)); 
              xx[1] = tp.fY + gRandom->Gaus(0., Tdiffusion(tp.fX)); 
              xx[2] = tp.fZ + gRandom->Gaus(0., Tdiffusion(tp.fX));

              qev->Record( Vdrift / xx[0], xx); 
            }

            primary_edep += tp.fEdep; 
          }

        }

        itrj++;
      } // end of loop over trajectories
    }
    q_ev_obs += primary_edep; 
    ip++;

  } // end of loop over primary particles

  qev->ApplySuppressionAndQRec(100); 

  return q_ev_obs; 
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

