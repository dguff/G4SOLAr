/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : test_output
 * @created     : mercoledì lug 20, 2022 16:40:43 CEST
 */

#include <iostream>
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"

#include "config/SLArCfgBaseSystem.hh"
#include "event/SLArMCEvent.hh"
#include "config/SLArCfgMegaTile.hh"
#include "SLArCfgSuperCellArray.hh"

#include "solar_root_style.hpp"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSCfg;
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;

void test_output(const char* path, int iev = 0) 
{
  slide_default(); 
  gROOT->SetStyle("slide_default");
  gStyle->SetPalette(kSunset); 

  TFile* file = new TFile(path); 

  SLArPDSCfg* pdsCfg = (SLArPDSCfg*)file->Get("PDSSysConfig"); 
  SLArPixCfg* pixCfg = (SLArPixCfg*)file->Get("PixSysConfig"); 

  if (pdsCfg) {
    for (auto &array : pdsCfg->GetModuleMap()) {
      SLArCfgSuperCellArray* scArray = array.second; 
      scArray->BuildPolyBinHist();
      new TCanvas(); 
      scArray->GetTH2()->Draw("col");
    }
  }

  std::map<int, TH2Poly*> hNPhMap; 
  std::map<int, TH2Poly*> hTPhMap; 

  if (pixCfg) {
    TH2D* h2frame = new TH2D("h2frame", "Pix Readout", 700, -7e3, 7e3, 600, -3e3, +3e3);
    new TCanvas(); 
    h2frame->Draw("axis"); 

    for (auto &mod : pixCfg->GetModuleMap()) {
      SLArCfgMegaTile* mgTile = mod.second; 
      //for (const auto &cell : mgTile->GetMap()) {
        //printf("cell pos [phys]: [%.2f, %.2f, %.2f]\n", 
            //cell.second->GetPhysX(), cell.second->GetPhysY(), cell.second->GetPhysZ()); 
      //}
      mgTile->BuildPolyBinHist(); 
      mgTile->GetTH2()->Draw("colsame"); 
      hNPhMap.insert(std::make_pair(mgTile->GetIdx(), 
            (TH2Poly*)mgTile->GetTH2()->Clone(Form("nhits_map_%i", mgTile->GetIdx()))));
      hTPhMap.insert(std::make_pair(mgTile->GetIdx(), 
            (TH2Poly*)mgTile->GetTH2()->Clone(Form("thits_map_%i", mgTile->GetIdx()))));
    }
  }

  TTree* tree = (TTree*)file->Get("EventTree"); 
  
  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 

  TH1D* hvis = new TH1D("hvis", "Visible Energy", 200, 0., 10); 
  TH1D* hNPhotons = new TH1D("hNPhotons", 
      "Nr of collected photons;Nr of collected photons (true);Entries/ev",
      100, 0, 25e3);
  TH1D* hTPhotons = new TH1D("hTPhotons", 
      "Time of photons hits;Time of first photon hit on sensor (true); Entries", 
      2000, 0, 3000); 
  
  double hmax = 0; 
  double tmax = 0; 

  for (int iev = 0; iev<tree->GetEntries(); iev++) {
    tree->GetEntry(iev); 

    auto primaries = ev->GetPrimaries(); 
    double ev_edep = 0; 

    //--------------------------------------------- Readout primaries and MC true
    size_t ip = 0; 
    for (const auto &p : primaries) {
      int pPDGID = p->GetCode();     // Get primary PDG code 
      int pTrkID = p->GetTrackID();  // Get primary trak id   

      auto trajectories = p->GetTrajectories(); 
      int itrj = 0;
      for (const auto &trj : trajectories) {
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
    hvis->Fill(ev_edep); 

    //------------------------------------------ Readout detected optical photons
    double htot = 0; 
    auto evpds = ev->GetReadoutTileSystem(); 
    for (const auto &mtile : evpds->GetMegaTilesMap()) {
      auto mgTileCfg = pixCfg->GetModule(mtile.first);
      for (const auto &tile : mtile.second->GetTileMap()) {
        SLArEventTile* evTile = tile.second; 
        if (!evTile->GetHits().empty()) {
          int tile_idx = evTile->GetIdx();
          int bin_idx = mgTileCfg->GetBaseElement(tile_idx)->GetBinIdx();
          int nhits = evTile->GetNhits(); 
          double tfirst = evTile->GetTime(); 
          if (tfirst > tmax) tmax = tfirst; 
          double bc_ = hNPhMap[mgTileCfg->GetIdx()]->GetBinContent(bin_idx); 
          hNPhMap[mgTileCfg->GetIdx()]->SetBinContent(bin_idx, bc_ + nhits);
          hTPhMap[mgTileCfg->GetIdx()]->SetBinContent(bin_idx, tfirst); 

          htot += nhits; 

          for (const auto &hit : evTile->GetHits()) {
            hTPhotons->Fill( hit->GetTime(), 1./tree->GetEntries() ); 
          }
        }
      }
    }

    hNPhotons->Fill(htot); 
  }

 
  TH2D* h2frame = new TH2D("h2frame", "Pix Readout", 700, -7e3, 7e3, 600, -3e3, +3e3);
  TCanvas* cPixN = new TCanvas("cPixN", "Pixel system readout - Nhits", 0, 0, 1500, 600);

  printf("---------------------- Drawing Nhits (hmax = %g)\n", hmax);
  cPixN->cd(); 
  int imap = 0; 
  h2frame->DrawClone("axis"); 
  for (const auto &hmap : hNPhMap) {
    hmap.second->Scale(1./tree->GetEntries()); 
    if (hmax < hmap.second->GetMaximum()) 
      hmax = hmap.second->GetMaximum(); 
  }

  for (auto &hmap : hNPhMap) {
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
  hNPhotons->Draw("hist");
  auto txt_nh = add_preliminary(0, 1); 
  txt_nh->Draw(); 

  TCanvas* cTime = new TCanvas("cTime", "cTime", 0, 0, 900, 600); 
  cTime->cd(); 
  hTPhotons->Draw("hist");
  auto txt_th = add_preliminary(1, 1); 
  txt_th->Draw(); 

  

  return;
}

