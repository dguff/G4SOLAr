/**
 * @author      Giulia Conti (unimib), Daniele Guffanti (unimin & infn-mib)
 * @file        electron_analysis.cc
 * @created     Wed Jul 26, 2023
 */

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "THnBase.h"
#include "THnSparse.h"
#include "TTimer.h"
#include "TMarker.h"

std::vector<Color_t> color_vector = {kOrange + 7, kRed, kMagenta + 1, kViolet + 6, kBlue, kAzure + 1, kCyan - 3, kGreen + 1, kGreen + 3, kYellow - 6};

Double_t CrystalBall(Double_t *x, Double_t *par)
{
  return par[4] * ROOT::Math::crystalball_function(x[0], par[0], par[1], par[2], par[3]);
}

Double_t Line(Double_t *x, Double_t *par)
{
  return par[0] * x[0] + par[1];
}

Double_t Calib_Line(Double_t *x)
{
  double m = 27296.6;
  double q = -309.885;
  return m * x[0] + q;
}

void electron_analysis()
{
  TString file_path[5] = {"/Users/giulia/Tesi/Eventi/elettroni/electrons_5_0MeV_iso_noise_900_processed.root",
                          "/Users/giulia/Tesi/Eventi/elettroni/electrons_7_5MeV_iso_noise_900_processed.root",
                          "/Users/giulia/Tesi/Eventi/elettroni/electrons_10_0MeV_iso_noise_900_processed.root",
                          "/Users/giulia/Tesi/Eventi/elettroni/electrons_12_5MeV_iso_noise_900_processed.root",
                          "/Users/giulia/Tesi/Eventi/elettroni/electrons_15_0MeV_iso_noise_900_processed.root"};

  TString energy_file[5] = {"5", "7.5", "10", "12.5", "15"};

  std::vector<TH1D *> h_tot_charge;
  std::vector<TH1D *> h_maxcl_charge;
  std::vector<TH1D *> cos_theta;
  std::vector<TH1D *> theta;

  for (int i = 0; i < 5; i++)
  {
    // printf("%s\n",file_path[i].Data());

    TFile *mc_file = new TFile(file_path[i].Data());
    TTree *mc_tree = (TTree *)mc_file->Get("processed_events");

    TString h_tot_charge_name = "h_qtot_" + energy_file[i] + "MeV";
    TString h_maxcl_charge_name = "h_qmaxcl_" + energy_file[i] + "MeV";
    TString h_cos_theta_name = "h_cos_th_" + energy_file[i] + "MeV";
    TString h_theta_name = "h_theta_" + energy_file[i] + "MeV";

    TH1D *h_qtot = new TH1D(h_tot_charge_name, "tot charge " + energy_file[i] + " MeV", 200, 0, 500e3);
    TH1D *h_q_maxcl = new TH1D(h_maxcl_charge_name, "max cluster charge " + energy_file[i] + " MeV", 200, 0, 500e3);
    TH1D *h_cos_th = new TH1D(h_cos_theta_name, "cos theta " + energy_file[i] + " MeV", 200, -1, 1);
    TH1D *h_theta = new TH1D(h_theta_name, "theta " + energy_file[i] + " MeV", 200, 0, 180);

    mc_tree->Draw("tot_charge>>" + h_tot_charge_name, "", "goff");
    mc_tree->Draw("cluster_charge>>" + h_maxcl_charge_name, "", "goff");
    mc_tree->Draw("cos_theta>>" + h_cos_theta_name, "", "goff");
    mc_tree->Draw("acos(cos_theta)*180/3.14>>" + h_theta_name, "", "goff");

    h_tot_charge.push_back(h_qtot);
    h_maxcl_charge.push_back(h_q_maxcl);
    cos_theta.push_back(h_cos_th);
    theta.push_back(h_theta);
  }

  TCanvas *C_tot_charge = new TCanvas("C_tot_charge", "Total charge", 0, 0, 1000, 600);

  for (int i = 0; i < 5; i++)
  {
    TH1D *h = h_tot_charge.at(i);
    // Opzioni grafiche
    h->SetLineColor(color_vector.at(i));
    h->Draw("same");
  }

  TCanvas *C_maxcl_charge = new TCanvas("C_maxcl_charge", "Max cluster charge", 0, 0, 1000, 600);

  for (int i = 0; i < 5; i++)
  {
    TH1D *h = h_maxcl_charge.at(i);
    // Opzioni grafiche
    h->SetLineColor(color_vector.at(i));
    h->Draw("same");
  }

  TCanvas *C_cos_theta = new TCanvas("C_cos_theta", "Cos theta", 0, 0, 1000, 600);

  for (int i = 0; i < 5; i++)
  {
    TH1D *h = cos_theta.at(i);
    // Opzioni grafiche
    h->SetFillColor(color_vector.at(i));
    h->GetYaxis()->SetRangeUser(0, 725);
    h->Draw("same");
  }

  TCanvas *C_theta = new TCanvas("C_theta", "Theta", 0, 0, 1000, 600);

  for (int i = 0; i < 5; i++)
  {
    TH1D *h = theta.at(i);
    // Opzioni grafiche
    h->SetFillColor(color_vector.at(i));
    h->GetYaxis()->SetRangeUser(0, 175);
    h->Draw("same");
  }

  TCanvas *C_theta_res = new TCanvas("C_theta_res", "Theta resolution", 0, 0, 1000, 600);
  TGraph *g_theta_res = new TGraph();

  for (int i = 0; i < 5; i++)
  {
    TH1D *h = theta.at(i);
    double q[1] = {0.68};
    double prob[1] = {0};
    h->GetQuantiles(1, prob, q);
    // Opzioni grafiche
    g_theta_res->AddPoint(std::atof(energy_file[i]), prob[0]);
  }
  g_theta_res->SetTitle("Theta resolution");
  g_theta_res->SetMarkerStyle(20);
  g_theta_res->Draw("AWPL");

  TCanvas *C_max_tot_charge = new TCanvas("C_max_tot_charge", "Max tot charge", 0, 0, 1000, 600);
  TGraph *g_max_charge = new TGraph();
  TGraph *g_charge_calib = new TGraph(); DA FINIRE
  TGraph *g_en_res = new TGraph();
  TF1 *fGauss = new TF1("fGauss", "gaus", 0, 500e3);
  TF1 *FCrystalBall = new TF1("FCrystalBall", CrystalBall, 0, 500e3, 5);
  TF1 *fLine = new TF1("fLine", Line, 0, 500e3, 2);
  FCrystalBall->SetNpx(1e4);

  for (int i = 0; i < 5; i++)
  {
    TH1D *h = h_tot_charge.at(i);
    // h->Fit(fGauss, "", "", h->GetBinCenter(h->GetMaximumBin())-20e3, h->GetBinCenter(h->GetMaximumBin())+200e3);
    FCrystalBall->SetParameters(1, 1, h->GetRMS(), h->GetMean(), 300);
    h->Fit(FCrystalBall, "Q");
    g_max_charge->AddPoint(std::atof(energy_file[i]), FCrystalBall->GetParameter(3));

    // g_en_res->AddPoint(std::atof(energy_file[i]), FCrystalBall->GetParameter(2)/FCrystalBall->GetParameter(3));
    double x_min_fwhm = FCrystalBall->GetX(0.5 * FCrystalBall->Eval(FCrystalBall->GetParameter(3)), 0, FCrystalBall->GetParameter(3));
    double x_maj_fwhm = FCrystalBall->GetX(0.5 * FCrystalBall->Eval(FCrystalBall->GetParameter(3)), FCrystalBall->GetParameter(3), 500e3);
    double fwhm = x_maj_fwhm - x_min_fwhm;
    g_en_res->AddPoint(std::atof(energy_file[i]), fwhm / FCrystalBall->GetParameter(3));

    // g_max_charge->AddPoint(std::atof(energy_file[i]),fGauss->GetParameter(1));
    // g_en_res->AddPoint(std::atof(energy_file[i]), fGauss->GetParameter(2)/fGauss->GetParameter(1));
    // auto I = fGauss->GetParameter(0);
    //  Opzioni grafiche

    // Cambiare colore fit
    FCrystalBall->SetLineColor(color_vector.at(i));
    FCrystalBall->SetLineStyle(7);
    FCrystalBall->DrawClone("same");
  }
  g_max_charge->SetTitle("Mean tot charge");
  g_max_charge->SetMarkerStyle(20);
  g_max_charge->Draw("AWPL"); // A disegna gli assi, PL come rappresenta i punti
  g_max_charge->Fit(fLine);

  TCanvas *C_tot_charge_en_res = new TCanvas("C_tot_charge_en_res", "Energy resolution total charge", 0, 0, 1000, 600);
  g_en_res->SetTitle("Energy resolution");
  g_en_res->SetMarkerStyle(20);
  g_en_res->Draw("AWPL"); // A disegna gli assi, PL come rappresenta i punti

  TCanvas *C_max_charge_maxcl = new TCanvas("C_max_charge_maxcl", "Max cluster charge", 0, 0, 1000, 600);
  TGraph *g_max_charge_maxcl = new TGraph();
  TGraph *g_en_res_maxcl = new TGraph();
  // TF1  *fGauss = new TF1("fGauss","gaus",0, 500e3);
  // TF1 *FCrystalBall = new TF1("FCrystalBall", CrystalBall, 0, 500e3, 5);
  FCrystalBall->SetNpx(1e4);

  for (int i = 0; i < 5; i++)
  {
    TH1D *h = h_maxcl_charge.at(i);
    // h->Fit(fGauss, "", "", h->GetBinCenter(h->GetMaximumBin())-20e3, h->GetBinCenter(h->GetMaximumBin())+200e3);
    FCrystalBall->SetParameters(1, 1, h->GetRMS(), h->GetMean(), 300);
    h->Fit(FCrystalBall, "Q");
    g_max_charge_maxcl->AddPoint(std::atof(energy_file[i]), FCrystalBall->GetParameter(3));

    double x_min_fwhm = FCrystalBall->GetX(0.5 * FCrystalBall->Eval(FCrystalBall->GetParameter(3)), 0, FCrystalBall->GetParameter(3));
    double x_maj_fwhm = FCrystalBall->GetX(0.5 * FCrystalBall->Eval(FCrystalBall->GetParameter(3)), FCrystalBall->GetParameter(3), 500e3);
    double fwhm = x_maj_fwhm - x_min_fwhm;
    g_en_res_maxcl->AddPoint(std::atof(energy_file[i]), fwhm / FCrystalBall->GetParameter(3));

    // g_en_res_maxcl->AddPoint(std::atof(energy_file[i]), FCrystalBall->GetParameter(2)/FCrystalBall->GetParameter(3));
    // g_max_charge->AddPoint(std::atof(energy_file[i]),fGauss->GetParameter(1));
    // g_en_res->AddPoint(std::atof(energy_file[i]), fGauss->GetParameter(2)/fGauss->GetParameter(1));
    // auto I = fGauss->GetParameter(0);
    //  Opzioni grafiche
  }
  g_max_charge_maxcl->SetTitle("Cluster mean tot charge");
  g_max_charge_maxcl->SetMarkerStyle(20);
  g_max_charge_maxcl->Draw("AWPL"); // A disegna gli assi, PL come rappresenta i punti

  TCanvas *C_en_res_maxcl = new TCanvas("C_en_res_maxcl", "Energy resolution", 0, 0, 1000, 600);
  g_en_res_maxcl->SetTitle("Cluster energy resolution");
  g_en_res_maxcl->SetMarkerStyle(20);
  g_en_res_maxcl->Draw("AWPL"); // A disegna gli assi, PL come rappresenta i punti
}

void neutrino_analysis()
{
  TString file_path = "/Users/giulia/Tesi/Eventi/neutrini/b8_nue_es_iso_total_noise_900_processed.root";

  TFile *mc_file = new TFile(file_path.Data());
  TTree *mc_tree = (TTree *)mc_file->Get("processed_events");

  TH1D *h_qtot = new TH1D("h_qtot", "tot charge", 200, 0, 500e3);
  TH1D *h_en = new TH1D("h_en", "energy", 200, 0, 500e3);
  TH1D *h_cos_th = new TH1D("h_cos_th", "cos theta", 200, -1, 1);
  TH1D *h_theta = new TH1D("h_theta", "theta", 200, 0, 180);

  mc_tree->Draw("tot_charge>>h_qtot", "", "goff");
  // Aggiungere h_en
  mc_tree->Draw("cos_theta>>h_cos_th", "", "goff");
  mc_tree->Draw("acos(cos_theta)*180/3.14>>h_theta", "", "goff");

}