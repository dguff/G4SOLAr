/**
 * @author      : Maryam Shooshtari (maryam@DESKTOP-7B1MNK7)
 * @file        : view_results
 * @created     : Monday Jul 04, 2022 16:49:45 CEST
 */

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "../include/SLArQConstants.h"


TH1D* view_results(const char* path, bool do_draw = true)
{
  TFile* file_in = new TFile(path);
  TTree* t =(TTree*) file_in->Get("light_tree");
  double cos_theta = 0;
  double charge = 0;
  int N_clusters = 0;
  t->SetBranchAddress("costheta", &cos_theta);
  t->SetBranchAddress("Qrec", &charge);
  t->SetBranchAddress("Ncluster", &N_clusters);
  TH1D* cos_theta_hist = new TH1D("cos_theta_hist","cos of scattering angle",100,-1,1);
  TH1D* Energy_hist = new TH1D("Energy","Total reconstructed energy of the event",100,0,20);

  for(int i =0; i<t->GetEntries(); i++){
    t->GetEntry(i);
    cos_theta_hist->Fill(cos_theta);
    Energy_hist->Fill(charge * W);


  }
  if (do_draw){
    TCanvas* c = new TCanvas("c","canvas",1000,600);
    c->Divide(2,1);
    c->cd(1);
    cos_theta_hist->Draw("hist");
    c->cd(2);
    Energy_hist->Draw("hist");
  }

  return cos_theta_hist;
}

int compare(){
  TString files_to_read[9] = {"./../data/electrons_20MeV_analysis.root" , "./../data/electrons_18MeV_analysis.root","./../data/electrons_16MeV_analysis.root", "./../data/electrons_14MeV_analysis.root", "./../data/electrons_12MeV_analysis.root", "./../data/electrons_10MeV_analysis.root", "./../data/electrons_8MeV_analysis.root", "./../data/electrons_6MeV_analysis.root", "./../data/electrons_4MeV_analysis.root"};
  Color_t col[9] = {kBlue, kRed, kMagenta, kOrange, kViolet+1, kYellow+3, kGreen+2, kCyan+2, kPink+8};
  int e[9] = {20,18,16,14,12,10,8,6,4};
  TCanvas* c1 = new TCanvas("c1", "another canvas", 800,500);
  c1->cd(1);
  TLegend* l = new TLegend(0.1,0.7,0.48,0.9);
  for (int j=0; j<9; j++){
    TH1D* temp_h = new TH1D();
    temp_h = view_results(files_to_read[j].Data(), false);
    temp_h->SetLineColor(col[j]);
    l->AddEntry(temp_h, Form("E = %i MeV", e[j]));
    if(j==5){
      temp_h->Draw("hist");
    }else{
      temp_h->Draw("hist same");
    }
    l->Draw();
    gPad->Modified();
    gPad->Update();
  }

  return 1;
}
