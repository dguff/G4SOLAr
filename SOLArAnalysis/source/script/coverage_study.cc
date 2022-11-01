/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : coverage_study.cc
 * @created     : mercoledì lug 20, 2022 16:40:43 CEST
 */

#include <getopt.h>
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TChain.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TH2Poly.h"
#include "TObjArray.h"
#include "TList.h"

#include "config/SLArCfgBaseSystem.hh"
#include "event/SLArMCEvent.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgSuperCellArray.hh"

#include "solar_root_style.hpp"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSCfg;
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;

const double avgLY         = 25980.5; //!< Average Light Yield
const double pdeSiPM       = 0.15;    //!< Photon Detection Efficiency for crosscheck
const double pdeSC         = 0.03;    //!< SuperCell Photon Detection Efficiency
const double PixFillFactor = 0.36*0.85; 
const double moduleSize[3] = {3600, 6000, 14000}; 
std::vector<double>  vBinsRT = {  2.50,  7.50, 12.50, 17.50, 22.50, 
                                 27.50, 32.50, 37.50, 42.50, 47.50, 52.50};
std::vector<double>  vBinsLT = { -1.25,  1.25,  3.75,  6.25,  8.75, 11.25,
                                 13.75, 17.50, 22.50, 27.50, 32.50}; 


struct  SLArHistoSet {
  public: 
    SLArHistoSet(); 
    ~SLArHistoSet(); 

    void Write(const char* output_path); 

    SLArPixCfg* fPixCfg; 
    SLArPDSCfg* fSCCfg; 
    TH3D* hVtxOrigin; 
    TH1D* hvis; 
    TH1D* hNPhotons; 
    std::vector<TH1D*> hPosition; 
    TH2D* hResVsCoverage;
    std::map<int, TH1D*> hResCoverage; 
    std::map<int, TH1D*> hEffLY;

}; 

SLArHistoSet::SLArHistoSet() : 
  fPixCfg(nullptr), fSCCfg(nullptr), 
  hVtxOrigin(nullptr), 
  hvis(nullptr), hNPhotons(nullptr), 
  hPosition(3, nullptr), hResVsCoverage(nullptr)
{
  hVtxOrigin = new TH3D("hVtxOrigin", "Vertex Origin;#it{x} [mm];#it{y} [mm];#it{z} [mm]", 
      moduleSize[0]/100, -0.5*moduleSize[0], +0.5*moduleSize[0], 
      moduleSize[1]/100, -0.5*moduleSize[1], +0.5*moduleSize[1], 
      moduleSize[2]/100, -0.5*moduleSize[2], +0.5*moduleSize[2]
      );
  
  hvis = new TH1D("hvis", "Visible Energy", 200, 0., 20); 
  hNPhotons = new TH1D("hNPhotons", 
      "Nr of photons produced;Nr of photons produced (true);Entries",
      1000, 0, 1e6);
  double  _dimensions[3] = {1.8, 3, 7}; 
  TString _positions[3] = {"x", "y", "z"};
  for (int i=0; i<3; i++) {
    hPosition[i] = new TH1D(Form("hPosition%s", _positions[i].Data()), 
        Form("Event starting point #it{%s};#it{%s} [mm];Entries", 
          _positions[i].Data(), _positions[i].Data()), 
        200, -_dimensions[i]*1000, _dimensions[i]*1000); 
  }

  hResVsCoverage = new TH2D("hResVsCoverage", 
      Form("%s;%s;%s;%s", "Energy resolution vs eff photo-coverage", 
        "Readout Tile coverage [%]", "Light Trap coverage [%]", "#it{#sigma}(#it{E}_{L}) [%]"), 
      vBinsRT.size()-1, &vBinsRT[0], vBinsLT.size()-1, &vBinsLT[0]);

  for (int ix=1; ix<=hResVsCoverage->GetNbinsX(); ix++) {
    for (int iy=1; iy<=hResVsCoverage->GetNbinsY(); iy++) {
      int ib = hResVsCoverage->GetBin(ix, iy); 
      double epsRT = hResVsCoverage->GetXaxis()->GetBinCenter(ix); 
      double epsLT = hResVsCoverage->GetYaxis()->GetBinCenter(iy); 

      hResCoverage.insert(
          std::make_pair(ib, 
            new TH1D(Form("hResCoverage%i", ib), 
              Form("Energy Resolution - #it{#varepsilon}_{RT} = %.2f%% - #it{#varepsilon}_{LT} = %.2f%%;#it{#delta}(E) [%%];Entries", 
                epsRT, epsLT), 
              1000, -50, +50)));
      hEffLY.insert(
          std::make_pair(ib, new TH1D(Form("hEffLY%i", ib), 
              Form("Effective LY - #it{#varepsilon}_{RT} = %.2f%% - #it{#varepsilon}_{LT} = %.2f%%;#it{#delta}(E) [%%];Entries", 
                epsRT, epsLT), 
              500, 0, 1000)));
    }
  }
}

SLArHistoSet::~SLArHistoSet() {
  if (hvis) delete hvis; 
  if (hNPhotons) delete hNPhotons; 
  if (hResVsCoverage) delete hResVsCoverage;
  for (auto &h : hPosition) {
    if (h) delete h; 
  }
  for (auto &h : hResCoverage) {
    if (h.second) delete h.second;
  }
  for (auto &h : hEffLY) {
    if (h.second) delete h.second; 
  }


  hPosition.clear(); 
  hResCoverage.clear(); 
  hEffLY.clear(); 
}

void SLArHistoSet::Write(const char* output_path) {
  TFile* output = new TFile(output_path, "recreate");
  output->cd(); 

  hVtxOrigin->Write(); 
  hvis->Write(); 
  hNPhotons->Write();
  hResVsCoverage->Write(); 

  for (int j=0; j<3; j++) {
    hPosition[j]->Write(); 
  }

  TDirectory* dRes = output->mkdir("ResHistMap");  
  dRes->cd(); 
  for (auto &h : hResCoverage) {
    h.second->Write(); 
  }
  output->cd(); 

  TDirectory* dEffLY = output->mkdir("EffectiveLY"); 
  dEffLY->cd(); 
  for (auto &h : hEffLY) {
    h.second->Write(); 
  }

  output->cd(); 
  
  output->Close(); 
}

void process_event_tree(TTree* tree, SLArHistoSet* h, TH3D* visPix, TH3D* visSC); 

void test_output(const char* input_path, const char* output_path = "", 
    TH3D* hvisPix = nullptr, TH3D* hvisSC = nullptr) 
{
  //--------------------------------------------------------- Source plot style 
  slide_default(); 
  gROOT->SetStyle("slide_default");
  gStyle->SetPalette(kSunset); 

  //-------------------------------------------------------------- Open MC file
  printf("Opening input file...\n");
  TFile* file = new TFile(input_path); 
  
  // create histograms
  printf("Building hist collection...\n");
  SLArHistoSet* h = new SLArHistoSet(); 

  //---------------------------------------------------- Readout the event tree
  printf("Getting data tree...\n");
  TTree* tree = (TTree*)file->Get("EventTree"); 

  process_event_tree(tree, h, hvisPix, hvisSC); 
  
  if ( (output_path != NULL) && (output_path[0] !=  '\0') ) {
    h->Write(output_path); 
  }
  return;
}

void merge_and_plot(const char* root_file_list, const char* output_path, 
    TH3D* hvisPix, TH3D* hvisSC) 
{
  slide_default(); 
  gROOT->SetStyle("slide_default"); 

  std::ifstream file_list; 
  file_list.open( root_file_list ); 
  if (!file_list.is_open()) {
    printf("%s root file list is not opened. Quit.\n", root_file_list);
    return;
  }

  SLArHistoSet* h = new SLArHistoSet(); 

  TChain* ch = new TChain("EventTree", "G4SOLAr event tree"); 
  std::string str; 
  int ifile = 0; 
  while ( std::getline(file_list, str) ) {
    printf("Adding to %s to chain\n", str.c_str());
    ch->AddFile(str.c_str());  
    
    ifile++; 
  } 

  process_event_tree(ch, h, hvisPix, hvisSC);

  if ( (output_path != NULL) && (output_path[0] !=  '\0') ) {
    h->Write(output_path); 
  }

  return; 
}

void process_event_tree(TTree* tree, SLArHistoSet* h, TH3D* hvisPix, TH3D* hvisSC)
{
  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 


  int N = TMath::Min(tree->GetEntries(), (Long64_t)10000); 

  for (int iev = 0; iev<N; iev++) {
    tree->GetEntry(iev); 

    if (iev%100 == 0) printf("processing ev %i\n", iev); 

    auto primaries = ev->GetPrimaries(); 
    double ev_edep = 0; 
    double primary_pos[3] = {0};

    //--------------------------------------------- Readout primaries and MC true
    size_t ip = 0; 
    int nphotons = 0; 
    double Etrue = 0; 
    for (const auto &p : primaries) {
      int pPDGID = p->GetCode();     // Get primary PDG code 
      int pTrkID = p->GetTrackID();  // Get primary trak id   
      Etrue += p->GetTotalEdep(); 

      nphotons += p->GetTotalScintPhotons(); 
      auto trajectories = p->GetTrajectories(); 
      int itrj = 0;
      for (const auto &trj : trajectories) {
        if (trj->GetTrackID() == pTrkID) {
          primary_pos[0] = trj->GetPoints().front().fX; 
          primary_pos[1] = trj->GetPoints().front().fY; 
          primary_pos[2] = trj->GetPoints().front().fZ; 

          for (int kk=0; kk<3; kk++) {
            h->hPosition[kk]->Fill(primary_pos[kk]); 
          }
          h->hVtxOrigin->Fill(primary_pos[0], primary_pos[1], primary_pos[2]); 
        }
        /*
         *for (const auto &tp : trj->GetPoints()) {
         *  double pos_x = tp.fX;     // x coordinate [mm]
         *  double pos_y = tp.fY;     // y coordinate [mm]
         *  double pos_z = tp.fZ;     // z coordinate [mm]
         *  double edep  = tp.fEdep;  // Energy deposited in the step [MeV]
         *  ev_edep += edep; 
         *}
         */
        itrj++;
      } 
      ip++;
    }
    h->hvis->Fill(ev_edep); 
    h->hNPhotons->Fill(nphotons); 

    //-------------------------------------------- Simulate photon propagation 
    //-------------------------------- and detection using the visibility maps
    double visRT = 0.; double epsRT = 1.0; 
    double visLT = 0.; double epsLT = 1.0; 

    if (hvisPix) {
      int ibin = hvisPix->FindBin(primary_pos[0], primary_pos[1], primary_pos[2]); 
      visRT = hvisPix->GetBinContent(ibin); 
    }

    if (hvisSC) {
      int ibin = hvisSC->FindBin(primary_pos[0], primary_pos[1], primary_pos[2]); 
      visLT = hvisSC->GetBinContent(ibin); 
    }

    double htotRT_expctd = nphotons*visRT*pdeSiPM;
    double htotLT_expctd  = nphotons*visLT*pdeSC; 

    for (int iRT = 1; iRT<=h->hResVsCoverage->GetNbinsX(); iRT++) {
      for (int iLT = 1; iLT<=h->hResVsCoverage->GetNbinsY(); iLT++) {
        epsRT = h->hResVsCoverage->GetXaxis()->GetBinCenter(iRT)*0.01; 
        epsLT = h->hResVsCoverage->GetYaxis()->GetBinCenter(iLT)*0.01; 

        double htotPix_ = gRandom->Poisson(htotRT_expctd*epsRT); 
        double htotLT_  = gRandom->Poisson(htotLT_expctd*epsLT); 
        
        // compute reconstructed energy
        double Eres = (htotPix_+htotLT_) / 
          (avgLY*(visRT*pdeSiPM*epsRT + visLT*pdeSC*epsLT) ) / Etrue * 100;
        //printf("hRT = %g, hLT = %g, Etrue = %g, epsLT = %g, epsRT = %g -> Eres = %g\n", 
            //htotPix_, htotLT_, Etrue, epsRT, epsLT, Eres);

        int resBin = h->hResVsCoverage->GetBin(iRT, iLT); 
        h->hResCoverage[resBin]->Fill(100-Eres); 

        h->hEffLY[resBin]->Fill( (htotPix_ + htotLT_) / Etrue); 
        //getchar(); 
      }
    }
  }

  // Fill TH2 bins 
  for (int ix=1; ix<=h->hResVsCoverage->GetNbinsX(); ix++) {
    for (int iy=1; iy<=h->hResVsCoverage->GetNbinsY(); iy++) {
      int ib = h->hResVsCoverage->GetBin(ix, iy); 
      double rms = h->hResCoverage[ib]->GetRMS(); 
      h->hResVsCoverage->SetBinContent(ib, rms); 
    }
  }

  return;
}

void PrintUsage() {
  printf("test_output usage:\n./test_output\n"); 
  printf("\t-i(--input) input_file\n"); 
  printf("\t-l(--list) input_list\n");
  printf("\t-o(--output) output_file\n");
  printf("\t-v(--visibility) visibility_file_map\n");
  printf("\t-h(--help) print usage\n\n"); 
  return; 
}

int main(int argc, char *argv[])
{
  const char* short_opts = "i:l:o:v:h";
  static struct option long_opts[6] = 
  {
    {"input"     , required_argument, 0, 'i'}, 
    {"list"      , required_argument, 0, 'l'}, 
    {"output"    , required_argument, 0, 'o'}, 
    {"visibility", required_argument, 0, 'v'}, 
    {"help"      , no_argument, 0, 'h'}, 
    {nullptr, no_argument, nullptr, 0}
  };

  int c, option_index; 

  const char* input_file_ = ""; 
  const char* input_list_ = ""; 
  const char* output_file_ = ""; 
  const char* visibility_file_ = ""; 

  while ( (c = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1) {
    switch(c) {
      case 'i' : {
        input_file_ = optarg;
        printf("input_file: %s\n", input_file_);
        break;
      }
      case 'o' : 
      {
        output_file_ = optarg; 
        printf("output_file: %s\n", output_file_);
        break;
      }
      case 'l' :
      {
        input_list_ = optarg; 
        break;
      }
      case 'v' : 
      {
        visibility_file_ = optarg; 
        break;
      }
      case 'h':
      {
        PrintUsage();
        return 4;
        break;
      }
    }
  }

  TString input_file = input_file_; 
  TString output_file = output_file_; 
  TString input_list = input_list_; 
  TString visibility_file = visibility_file_; 

  TH3D* hvisPix = nullptr; 
  TH3D* hvisSC  = nullptr; 
  if (!visibility_file.IsNull()) {
    TFile* file = new TFile(visibility_file); 
    hvisPix = (TH3D*)file->Get("hvisPix")->Clone(); 
    hvisSC  = (TH3D*)file->Get("hvisSC")->Clone(); 
  }

  printf("input file: %s\n", input_file.Data()); 
  
  if (input_list.IsNull() && !input_file.IsNull()) {
    test_output(input_file.Data(), output_file.Data(), hvisPix, hvisSC); 
  } else if (!input_list.IsNull()) {
    merge_and_plot(input_list.Data(), output_file.Data(), hvisPix, hvisSC); 
  } else {
    printf("No valid input provided\n");
    PrintUsage(); 
  }
  return 0;
}

void plot_contours(const char* hist_file_path, double E = 0) {
  TFile* ff = new TFile(hist_file_path); 
  TH2D* h2 = (TH2D*)ff->Get("hResVsCoverage"); 
  TH2D* h2cnt = (TH2D*)h2->Clone(); 

  std::vector<double> vCntLv = {3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0}; 
  std::vector<TGraph*> vCnt; 
  h2cnt->SetContour(vCntLv.size(), &vCntLv[0]); 

  TCanvas* cTmp = new TCanvas("cTmp", "c", 0, 0, 600, 600); 
  cTmp->cd(); 
  h2cnt->Draw("cont list"); 
  cTmp->Update(); 

  TObjArray *contours = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");
  Int_t ncontours     = contours->GetSize();
  for (int i=0; i<ncontours; i++) {
    TList* cntlist      = (TList*)contours->At(i); 
    if (cntlist->First()) {
      vCnt.push_back( (TGraph*)cntlist->First()->Clone() ); 
      vCnt.back()->SetNameTitle(Form("cnt_%i", i+1), Form("%g %%", vCntLv.at(i+1))); 
    }
  }

  slide_default(); 
  gROOT->SetStyle("slide_default"); 
  gStyle->SetOptStat(0); 
  gStyle->SetOptTitle(0);
  TColor::InvertPalette(); 
  TCanvas* cPlot = new TCanvas("cPlot", "cPlot", 800, 600); 
  cPlot->SetTopMargin(0.05); 
  cPlot->SetRightMargin(0.15);
  cPlot->SetLeftMargin(0.15); 
  cPlot->SetBottomMargin(0.12); 

  h2->SetXTitle("Anode photocoverage [%]"); 
  h2->SetYTitle("Field cage (top/bottom) photocoverage [%]"); 
  h2->GetXaxis()->CenterTitle(); 
  h2->GetYaxis()->CenterTitle(); 
  h2->GetZaxis()->CenterTitle(); 
  h2->Draw("colz"); 
  h2->GetZaxis()->SetRangeUser(2., 9.); 

  TLatex ll; 
  ll.SetTextSize(23); 
  ll.SetTextFont(43); 
  ll.SetTextColor(kBlack); 
  ll.SetTextAlign(33); 

  ll.DrawLatexNDC(0.83, 0.93, Form("#it{E} = %g MeV", E)); 
  //for (int j=0; j<ncontours; j++) {
    //if (vCnt.at(j)) {
      //vCnt.at(j)->SetLineColor( gStyle->GetColorPalette((ncontours-j) * 255 / ncontours)); 
      //vCnt.at(j)->Draw("l same"); 
    //} else {
      //printf("vCnt[%i] is null!\n", j);
    //}
  //}

  
}
