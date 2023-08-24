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
std::vector<Color_t> color_vector_nu = {kCyan - 3, kGreen - 2, kOrange + 7, kMagenta + 1, kBlue};
  //kBlue, kViolet + 6, kMagenta + 1, kRed, kOrange + 7};

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
  double m = 3.66331e-05;
  double q = 0.011406;
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
  TGraph *g_charge_calib = new TGraph();
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
    g_charge_calib->AddPoint(FCrystalBall->GetParameter(3), std::atof(energy_file[i]));

    // g_en_res->AddPoint(std::atof(energy_file[i]), FCrystalBall->GetParameter(2)/FCrystalBall->GetParameter(3));
    double x_min_fwhm = FCrystalBall->GetX(0.5 * FCrystalBall->Eval(FCrystalBall->GetParameter(3)), 0, FCrystalBall->GetParameter(3));
    double x_maj_fwhm = FCrystalBall->GetX(0.5 * FCrystalBall->Eval(FCrystalBall->GetParameter(3)), FCrystalBall->GetParameter(3), 500e3);
    double fwhm = x_maj_fwhm - x_min_fwhm;
    g_en_res->AddPoint(std::atof(energy_file[i]), fwhm / FCrystalBall->GetParameter(3));

    // g_max_charge->AddPoint(std::atof(energy_file[i]),fGauss->GetParameter(1));
    // g_en_res->AddPoint(std::atof(energy_file[i]), fGauss->GetParameter(2)/fGauss->GetParameter(1));
    // auto I = fGauss->GetParameter(0);
    // Opzioni grafiche

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

  printf("Fit energy vs mean tot charge\n");
  g_charge_calib->Fit(fLine);

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
  // Create vectors for hist with different energy range
  std::vector<TH1D *> cos_theta;
  std::vector<TH1D *> theta;
  std::vector<TH2D *> en_vs_cos_theta;
  std::vector<float> theta_res;


  // Read data processed file
  TString file_path = "/Users/giulia/Tesi/Eventi/neutrini/b8_nue_es_iso_total_noise_900_processed.root";

  TFile *mc_file = new TFile(file_path.Data());
  TTree *mc_tree = (TTree *)mc_file->Get("processed_events");


  // Create and define hist (without energy range)
  TH1D *h_qtot = new TH1D("h_qtot", "tot charge", 200, 0, 500e3);
  TH1D *h_en_reco = new TH1D("h_en_reco", "reconstructed energy", 200, 0, 30);
  TH2D *h_cos_vs_en = new TH2D("h_cos_vs_en", "Energy vs cos theta", 100, -1, 1, 100, 0, 30);
  // --> TH2D *h_en_true_vs_reco = new TH2D("h_en_true_vs_reco", "Energy true vs reconstructed", 100, 0, 35, 100, 0, 35);
  // --> TH2D *h_en_visb_vs_reco = new TH2D("h_en_visb_vs_reco", "Energy visible vs reconstructed", 100, 0, 35, 100, 0, 35);

  double m = 3.66331e-05;
  double q0 = 0.011406;
  TString draw_en_calib = Form("%f * tot_charge + %f", m, q0);


  mc_tree->Draw("tot_charge>>h_qtot", "", "goff");
  mc_tree->Draw(draw_en_calib + ">>h_en_reco", "", "goff");
  mc_tree->Draw(draw_en_calib + ":cos_theta>>h_cos_vs_en", "", "goff");
  // --> mc_tree->Draw("true_energy:draw_en_calib>>h_en_true_vs_reco", "", "goff");
  // --> mc_tree->Draw("visb_energy:draw_en_calib>>h_en_visb_vs_reco", "", "goff");
    // --> Metto la soglia?
  
  // Create and define hist (with threshold)
  TH1D *h_qtot_th = new TH1D("h_qtot_th", "tot charge th", 200, 0, 500e3);
  TH1D *h_en_reco_th = new TH1D("h_en_reco_th", "reconstructed energy th", 200, 0, 30);
  TH2D *h_cos_vs_en_th = new TH2D("h_cos_vs_en_th", "Energy vs cos theta th", 100, -1, 1, 100, 0, 30);

  mc_tree->Draw("tot_charge>>h_qtot_th", draw_en_calib + ">5", "goff");
  mc_tree->Draw(draw_en_calib + ">>h_en_reco_th", draw_en_calib + ">5", "goff");
  mc_tree->Draw(draw_en_calib + ":cos_theta>>h_cos_vs_en_th", draw_en_calib + ">5", "goff");


  double q_en[5] = {0.2, 0.4, 0.6, 0.8, 1};
  double prob_en[5] = {0,0,0,0,0};
  h_en_reco_th->GetQuantiles(5, prob_en, q_en);
  printf("Energie quantili: %f, %f, %f, %f, %f\n", prob_en[0],prob_en[1],prob_en[2],prob_en[3],prob_en[4]);

  TH1D * h_cos_th[5];
  TH1D * h_theta[5];
  TH2D * h_en_vs_cos_theta[5];

  double q68[1] = {0.68};
  double prob68[1] = {0};

  for (int i = 0; i < 5; i++)
  {
    float E0 = 0;
    float E1 = prob_en[i]; 
    
    if (i == 0) E0 = 5;
    else E0 = prob_en[i-1];

    // Define and create hist with energy range
    TString h_cos_th_name = Form("h_cos_th_q%i", i);
    TString h_cos_th_title = Form("Cos#theta - en[%f,%f]", E0, E1);
    h_cos_th[i] = new TH1D(h_cos_th_name, h_cos_th_title, 100, -1, 1);

    TString h_theta_name = Form("h_theta_q%i", i);
    TString h_theta_title = Form("#theta - en[%f,%f]", E0, E1);
    h_theta[i] = new TH1D(h_theta_name, h_theta_title, 100, 0, 180);

    TString h_en_vs_cos_theta_name = Form("h_en_vs_cos_theta_q%i", i);
    TString h_en_vs_cos_theta_title = Form("Energy vs cos#theta - en[%f,%f]", E0, E1);
    h_en_vs_cos_theta[i] = new TH2D(h_en_vs_cos_theta_name, h_en_vs_cos_theta_title, 100, -1, 1, 100, 0, 18);
    
    TString draw_en_quant_min = Form("%f", E0);
    TString draw_en_quant_max = Form("%f", E1);

    TString draw_en_section = draw_en_calib + " > " + draw_en_quant_min + " && " + draw_en_calib + " < " + draw_en_quant_max;
    // printf("%s\n", draw_en_section.Data());

    mc_tree->Draw("cos_theta>>"+h_cos_th_name, draw_en_section, "goff");
    mc_tree->Draw("acos(cos_theta)*180/3.14>>"+h_theta_name, draw_en_section, "goff");
    mc_tree->Draw(draw_en_calib + ":cos_theta>>"+h_en_vs_cos_theta_name, draw_en_section, "goff");

    // Push these hists in their vectors
    cos_theta.push_back(h_cos_th[i]);
    theta.push_back(h_theta[i]);
    en_vs_cos_theta.push_back(h_en_vs_cos_theta[i]);

    h_theta[i]->GetQuantiles(1, prob68, q68);
    // Opzioni grafiche
    theta_res.push_back(prob68[0]);
  }

  printf("theta resolution: %f, %f, %f, %f, %f\n", theta_res[0], theta_res[1], theta_res[2], theta_res[3], theta_res[4]);

  // Canva without threshold 
  TCanvas *C_tot_charge = new TCanvas("C_tot_charge", "Total charge", 0, 0, 1000, 600);
  TH1D *h1 = h_qtot;
  h1->SetLineColor(kBlue);
  h1->Draw();

  TCanvas *C_en_reco = new TCanvas("C_en_reco", "Reconstructed energy", 0, 0, 1000, 600);
  TH1D *h0 = h_en_reco;
  h0->SetLineColor(kBlue);
  h0->Draw();

  TCanvas *C_cos_vs_en = new TCanvas("C_cos_vs_en", "Energy vs cos theta", 0, 0, 1000, 600);
  TH2D *h2 = h_cos_vs_en;
  h2->Draw("colz");

  // TCanvas *C_cos_vs_en = new TCanvas("C_cos_vs_en", "Energy vs cos theta", 0, 0, 1000, 600);
  // TH2D *h2 = en_vs_cos_theta.at(0);
  // h2->Draw("colz");

  // --> TCanvas *C_en_true_vs_reco = new TCanvas("C_en_true_vs_reco", "Energy true vs reco", 0, 0, 1000, 600);
  // TH2D *h3 = h_en_true_vs_reco;
  // h3->Draw("colz");

  // --> TCanvas *C_en_visb_vs_reco = new TCanvas("C_en_visb_vs_reco", "Energy visible vs reco", 0, 0, 1000, 600);
  // TH2D *h4 = h_en_visb_vs_reco;
  // h4->Draw("colz");


  // Canva with threshold
  TCanvas *C_tot_charge_th = new TCanvas("C_tot_charge_th", "Total charge th", 0, 0, 1000, 600);
  TH1D *h1_th = h_qtot_th;
  h1_th->SetLineColor(kBlue);
  h1_th->Draw();

  TCanvas *C_en_reco_th = new TCanvas("C_en_reco_th", "Reconstructed energy th", 0, 0, 1000, 600);
  TH1D *h0_th = h_en_reco_th;
  h0_th->SetLineColor(kBlue);
  h0_th->Draw();

  TCanvas *C_cos_vs_en_th = new TCanvas("C_cos_vs_en_th", "Energy vs cos theta th", 0, 0, 1000, 600);
  TH2D *h2_th = h_cos_vs_en_th;
  h2_th->Draw("colz");


  TCanvas *C_cos_theta = new TCanvas("C_cos_theta", "Cos theta", 0, 0, 1000, 600);
  TLegend *legend1 = new TLegend(0.7, 0.7, 0.9, 0.9);
  legend1->SetHeader("Energy");
  for (int i = 0; i < 5; i++)
  {
    TH1D *h = cos_theta.at(i);
    // Opzioni grafiche
    //h->SetFillStyle(3005);
    //h->SetFillColor(color_vector_nu.at(i));
    h->SetFillColorAlpha(color_vector_nu.at(i), 0.4);
    h->GetYaxis()->SetRangeUser(0, 550);
    h->SetTitle("cos#theta");
    h->Draw("same");

    float E0 = 0;
    float E1 = prob_en[i]; 
    if (i == 0) E0 = 5;
    else E0 = prob_en[i-1];
    legend1->AddEntry(h, Form("[%f, %f]", E0, E1), "f"); // "f" indica un oggetto con riempimento
  }
    legend1->Draw();


  TCanvas *C_theta = new TCanvas("C_theta", "Theta", 0, 0, 1000, 600);
  TLegend *legend2 = new TLegend(0.7, 0.7, 0.9, 0.9);
  legend2->SetHeader("Energy");
  for (int i = 0; i < 5; i++)
  {
    TH1D *h = theta.at(i);
    // Opzioni grafiche
    // h->SetFillStyle(4090);
    // h->SetFillColor(color_vector_nu.at(i));
    h->SetFillColorAlpha(color_vector_nu.at(i), 0.4);
    h->GetYaxis()->SetRangeUser(0, 220);
    h->SetTitle("#theta");
    h->Draw("same");

    float E0 = 0;
    float E1 = prob_en[i]; 
    if (i == 0) E0 = 5;
    else E0 = prob_en[i-1];
    legend2->AddEntry(h, Form("[%f, %f]", E0, E1), "f"); // "f" indica un oggetto con riempimento
  }
  legend2->Draw();

  
  for (int i = 0; i < 5; i++)
  {
    TString title_en_quant_min = Form("%f", prob_en[i-1]);
    TString title_en_quant_max = Form("%f", prob_en[i]);

    TString title_en_section = "";

    if (i==0) title_en_section = "[5,"+title_en_quant_max+"]";
    else title_en_section = "[" + title_en_quant_min + "," + title_en_quant_max + "]";

    TString canva_name = "C_en_vs_cos_theta_en"+title_en_section;
    TCanvas *C_en_vs_cos_theta = new TCanvas(canva_name, canva_name, 0, 0, 1000, 600);  
    TH2D *h = en_vs_cos_theta.at(i);
    // Opzioni grafiche
    h->SetTitle(canva_name);
    h->Draw("colz");
  }

}