/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : test_output
 * @created     : mercoledì lug 20, 2022 16:40:43 CEST
 */

#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TROOT.h"
#include "TChain.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TH2Poly.h"

#include "config/SLArCfgBaseSystem.hh"
#include "event/SLArMCEvent.hh"
#include "config/SLArCfgMegaTile.hh"
#include "SLArCfgSuperCellArray.hh"

#include "solar_root_style.hpp"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSCfg;
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;

struct  SLArHistoSet {
  public: 
    SLArHistoSet(); 
    ~SLArHistoSet(); 

    int BuildHitMapHist(SLArPixCfg* pixCfg); 

    void Write(const char* output_path); 

    SLArPixCfg* fPixCfg; 
    std::map<int, TH2Poly*> hNPhMap; 
    std::map<int, TH2Poly*> hTPhMap; 
    TH1D* hvis; 
    TH1D* hNPhotons; 
    TH1D* hNHits;
    TH1D* hTHits; 
    TH1D* hWavelength; 
    std::vector<TH1D*> hPosition; 

}; 

SLArHistoSet::SLArHistoSet() : 
  fPixCfg(nullptr), hvis(nullptr), hNPhotons(nullptr), 
  hNHits(nullptr), hTHits(nullptr), hWavelength(nullptr), 
  hPosition(3, nullptr)
{
  hvis = new TH1D("hvis", "Visible Energy", 200, 0., 20); 
  hNPhotons = new TH1D("hNPhotons", 
      "Nr of photons produced;Nr of photons produced (true);Entries",
      1000, 0, 1e6);
  hNHits = new TH1D("hNHits", 
      "Nr of collected photons;Nr of collected photons (true);Entries",
      500, 0, 1e5);
  hTHits = new TH1D("hTHits", 
      "Time of photons hits;Time of first photon hit on sensor (true);Entries", 
      2000, 0, 3000); 
  hWavelength = new TH1D("hWavelength", 
      "Wavelength of detected ph;#it{#lambda} [nm];Entries", 
      500, 100, 400); 

  double  _dimensions[3] = {1.8, 3, 7}; 
  TString _positions[3] = {"x", "y", "z"};
  for (int i=0; i<3; i++) {
    hPosition[i] = new TH1D(Form("hPosition%s", _positions[i].Data()), 
        Form("Event starting point #it{%s};#it{%s} [mm];Entries", 
          _positions[i].Data(), _positions[i].Data()), 
        200, -_dimensions[i]*1000, _dimensions[i]*1000); 
  }
}

SLArHistoSet::~SLArHistoSet() {
  if (hvis) delete hvis; 
  if (hNHits) delete hNHits;
  if (hNPhotons) delete hNPhotons; 
  if (hTHits) delete hTHits; 
  if (hWavelength) delete hWavelength; 
  for (auto &h : hPosition) {
    if (h) delete h; 
  }
  hPosition.clear(); 
}

int SLArHistoSet::BuildHitMapHist(SLArPixCfg* pixCfg) {
  fPixCfg = new SLArPixCfg( *pixCfg );
  int nmodules = 0; 
  for (auto &mod : fPixCfg->GetModuleMap()) {
    SLArCfgMegaTile* mgTile = mod.second; 
    for (auto &cell : mgTile->GetMap()) {
      cell.second->BuildGShape(); 
      //printf("cell pos [phys]: [%.2f, %.2f, %.2f] - %g × %g\n", 
          //cell.second->GetPhysX(), cell.second->GetPhysY(), cell.second->GetPhysZ(),
          //cell.second->Get2DSize_X(), cell.second->Get2DSize_Y()); 
    }
    mgTile->BuildPolyBinHist(); 
    TH2Poly* h2p = (TH2Poly*)mgTile->GetTH2(); 
    hNPhMap.insert(std::make_pair(mgTile->GetIdx(), 
          (TH2Poly*)h2p->Clone(Form("nhits_map_%i", mgTile->GetIdx()))));
    hTPhMap.insert(std::make_pair(mgTile->GetIdx(), 
          (TH2Poly*)h2p->Clone(Form("thits_map_%i", mgTile->GetIdx()))));
  }
  nmodules = hNPhMap.size(); 
  return nmodules; 
}

void SLArHistoSet::Write(const char* output_path) {
  TFile* output = new TFile(output_path, "recreate");

  hvis->Write(); 
  hNPhotons->Write();
  hNHits->Write(); 
  hTHits->Write(); 
  hWavelength->Write(); 

  TH2D* h2hits = new TH2D("h2hits_avg", "Average nr of hits;#it{z} [mm];#it{y} [mm]", 
      140, -7000, +7000, 60, -3000, +3000); 
  for (const auto &mtile : hNPhMap) {
    for (const auto &&tbin : *(mtile.second->GetBins())) {
      TH2PolyBin* bin = (TH2PolyBin*)tbin; 
      TGraph* g = (TGraph*)bin->GetPolygon();
      double _x = 0.5*(g->GetPointX(0) + g->GetPointX(2)); 
      double _y = 0.5*(g->GetPointY(0) + g->GetPointY(2)); 
      //printf("_x = 0.5*(%g + %g)\n_y = 0.5*(%g + %g)\n\n", 
          //g->GetPointX(0), g->GetPointX(1), 
          //g->GetPointY(0), g->GetPointY(2));
      int iibin = h2hits->FindBin(_x, _y);
      h2hits->SetBinContent(iibin, bin->GetContent()); 
    }
  }

  h2hits->Write(); 

  output->Close(); 
}

void readout_event_tree(TTree* tree, SLArPixCfg* pixCfg, SLArHistoSet* h); 

void test_output(const char* path, const char* output_path = "") 
{
  //--------------------------------------------------------- Source plot style 
  slide_default(); 
  gROOT->SetStyle("slide_default");
  gStyle->SetPalette(kSunset); 

  //-------------------------------------------------------------- Open MC file
  TFile* file = new TFile(path); 
  
  // Get the configuration of the pixel/SuperCell readout system
  SLArPixCfg* pixCfg = (SLArPixCfg*)file->Get("PixSysConfig"); 


  // create histograms
  SLArHistoSet* h = new SLArHistoSet(); 
  if (pixCfg) {
    h->BuildHitMapHist(pixCfg); 
  }


  //---------------------------------------------------- Readout the event tree
  TTree* tree = (TTree*)file->Get("EventTree"); 

  readout_event_tree(tree, pixCfg, h); 
  
  if ( (output_path != NULL) && (output_path[0] !=  '\0') ) {
    h->Write(output_path); 
  }
  return;
}

void merge_and_plot(const char* root_file_list, const char* output_path) {
  slide_default(); 
  gROOT->SetStyle("slide_default"); 

  std::ifstream file_list; 
  file_list.open( root_file_list ); 
  if (!file_list.is_open()) {
    printf("%s root file list is not opened. Quit.\n", root_file_list);
    return;
  }

  SLArPixCfg* pixCfg = new SLArPixCfg();
  SLArHistoSet* h = new SLArHistoSet(); 

  TChain* ch = new TChain("EventTree", "G4SOLAr event tree"); 
  std::string str; 
  int ifile = 0; 
  while ( std::getline(file_list, str) ) {
    if (ifile == 0) {
      printf("Reading readout configuration from %s\n", str.c_str());
      TFile* f = new TFile(str.c_str()); 
      pixCfg = (SLArPixCfg*)f->Get("PixSysConfig")->Clone("local_pix_cfg");

      h->BuildHitMapHist(pixCfg); 
    }

    printf("Adding to %s to chain\n", str.c_str());
    ch->AddFile(str.c_str());  
    
    ifile++; 
  } 

  readout_event_tree(ch, pixCfg, h);

  if ( (output_path != NULL) && (output_path[0] !=  '\0') ) {
    h->Write(output_path); 
  }

  return; 
}

void readout_event_tree(TTree* tree, SLArPixCfg* pixCfg, SLArHistoSet* h) {
  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 

  double tmax = 0; 
  double hmax = 0; 

  for (int iev = 0; iev<tree->GetEntries(); iev++) {
    tree->GetEntry(iev); 

    auto primaries = ev->GetPrimaries(); 
    double ev_edep = 0; 

    //--------------------------------------------- Readout primaries and MC true
    size_t ip = 0; 
    int nphotons = 0; 
    for (const auto &p : primaries) {
      int pPDGID = p->GetCode();     // Get primary PDG code 
      int pTrkID = p->GetTrackID();  // Get primary trak id   

      nphotons += p->GetTotalScintPhotons(); 
      auto trajectories = p->GetTrajectories(); 
      int itrj = 0;
      for (const auto &trj : trajectories) {
        if (trj->GetTrackID() == pTrkID) {
          h->hPosition[0]->Fill(trj->GetPoints().front().fX); 
          h->hPosition[1]->Fill(trj->GetPoints().front().fY); 
          h->hPosition[2]->Fill(trj->GetPoints().front().fZ); 
        }
        for (const auto &tp : trj->GetPoints()) {
          double pos_x = tp.fX;     // x coordinate [mm]
          double pos_y = tp.fY;     // y coordinate [mm]
          double pos_z = tp.fZ;     // z coordinate [mm]
          double edep  = tp.fEdep;  // Energy deposited in the step [MeV]
          ev_edep += edep; 
        }
        itrj++;
      } 
      ip++;
    }
    h->hvis->Fill(ev_edep); 
    h->hNPhotons->Fill(nphotons); 

    //---------------------------------------- Readout detected optical photons
    double htot = 0; 
    auto evpds = ev->GetReadoutTileSystem(); 
    for (const auto &mtile : evpds->GetMegaTilesMap()) {
      auto mgTileCfg = h->fPixCfg->GetModule(mtile.first);
      for (const auto &tile : mtile.second->GetTileMap()) {
        SLArEventTile* evTile = tile.second; 
        if (!evTile->GetHits().empty()) {
          int tile_idx = evTile->GetIdx();
          int bin_idx = mgTileCfg->GetBaseElement(tile_idx)->GetBinIdx();
          int nhits = evTile->GetNhits(); 
          double tfirst = evTile->GetTime(); 
          if (tfirst > tmax) tmax = tfirst; 
          double bc_ = h->hNPhMap[mgTileCfg->GetIdx()]->GetBinContent(bin_idx); 
          h->hNPhMap[mgTileCfg->GetIdx()]->SetBinContent(bin_idx, bc_ + nhits);
          h->hTPhMap[mgTileCfg->GetIdx()]->SetBinContent(bin_idx, tfirst); 

          htot += nhits; 

          for (const auto &hit : evTile->GetHits()) {
            h->hTHits->Fill( hit->GetTime(), 1./tree->GetEntries() ); 
            h->hWavelength->Fill( hit->GetWavelength()); 
          }
        }
      }
    }

    h->hNHits->Fill(htot); 
  }

  printf("------------------------------------------ Drawing MC truth info\n");
  TCanvas* cVisibleEnergy = new TCanvas("cVisibleEnergy", "cVisibleEnergy", 0, 0, 800, 500);
  cVisibleEnergy->Divide(2, 1); 
  cVisibleEnergy->cd(1); 
  h->hvis->Draw("hist");
  cVisibleEnergy->cd(2); 
  h->hNPhotons->Draw("hist"); 
  TCanvas* cPosition = new TCanvas("cPosition", "cPosition", 0, 0, 1200, 500); 
  cPosition->Divide(3, 1); 
  for (int j=0; j<3; j++) {
    cPosition->cd(j+1); 
    h->hPosition[j]->Draw("hist"); 
  }
 

  printf("------------------------------------------ Drawing Nhits (hmax = %g)\n",
      hmax);
  TH2D* h2frame = new TH2D("h2frame", "Pix Readout", 700, -7e3, 7e3, 600, -3e3, +3e3);
  TCanvas* cPixN = new TCanvas("cPixN", "Pixel system readout - Nhits", 0, 0, 1500, 600);

  cPixN->cd(); 
  int imap = 0; 
  h2frame->DrawClone("axis"); 
  for (const auto &hmap : h->hNPhMap) {
    hmap.second->Scale(1./tree->GetEntries()); 
    if (hmax < hmap.second->GetMaximum()) 
      hmax = hmap.second->GetMaximum(); 
  }

  for (auto &hmap : h->hNPhMap) {
    hmap.second->GetZaxis()->SetRangeUser(0, 1.1*hmax); 
    if (imap == 0) hmap.second->Draw("colz0 same"); 
    else hmap.second->Draw("col0 same"); 
    imap++; 
  }
  auto txt_hmap = add_preliminary(0, 1); 
  txt_hmap->SetTextSize(30);
  txt_hmap->SetX(gStyle->GetPadLeftMargin()); 
  txt_hmap->SetY(1-gStyle->GetPadTopMargin()+0.02);
  txt_hmap->SetTextAlign(11); 
  txt_hmap->Draw(); 

  TCanvas* cNhits = new TCanvas("cNhits", "cNhits", 0, 0, 600, 600); 
  cNhits->cd(); 
  h->hNHits->Draw("hist");
  auto txt_nh = add_preliminary(0, 1); 
  txt_nh->Draw(); 

  TCanvas* cTime = new TCanvas("cTime", "cTime", 0, 0, 900, 600); 
  cTime->cd(); 
  h->hTHits->Draw("hist");
  auto txt_th = add_preliminary(1, 1); 
  txt_th->Draw(); 

  TCanvas* cWavelength = new TCanvas("cWavelength", "cWavelength", 0, 0, 600, 600); 
  cWavelength->cd(); 
  h->hWavelength->Draw("hist");

  return;
}
