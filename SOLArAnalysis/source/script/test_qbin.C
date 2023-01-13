/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : test_qbin
 * @created     : mercoledì nov 09, 2022 12:47:46 CET
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
#include "TDatabasePDG.h"

#include "config/SLArCfgSystemPix.hh"
#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "event/SLArMCEvent.hh"
#include "event/SLArEventMegatile.hh"
#include "event/SLArEventTile.hh"



void test_qbin(const char* input_path, int iev = 0) 
{
  gStyle->SetPalette(kBlackBody); 
  TColor::InvertPalette(); 
  TFile* file = new TFile(input_path); 
  TTree* tree = (TTree*)file->Get("EventTree"); 
  SLArCfgSystemPix* anodeCfg = (SLArCfgSystemPix*)file->Get("PixSysConfig"); 

  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 
  tree->GetEntry(iev); 
  double hmax = 0.;
  double htmax = 0.; 

  // read events
  std::vector<TH2Poly*> tileMaps; 
  std::vector<TH2Poly*> pixMaps;
  tileMaps.reserve(200);
  pixMaps.reserve(200); 

  auto anodeEv = ev->GetReadoutTileSystem(); 

  for (const auto& mtEv : anodeEv->GetMegaTilesMap()) {
    auto mt_hits = mtEv.second->GetNChargeHits(); 
    printf("Collected charge: %i\n", mt_hits);
    if (mt_hits == 0) continue;

    TH2Poly* h2Tile = anodeCfg->ConstructPixHistMap(1, std::vector<int>(1,mtEv.first)); 
    const auto mtCfg = anodeCfg->GetBaseElement(mtEv.first); 

    for (const auto& tEv : mtEv.second->GetTileMap()) {
      auto t_hits = tEv.second->GetPixelHits();
      if (t_hits == 0) continue;

      if (t_hits > htmax) htmax = t_hits; 
      h2Tile->SetBinContent(mtCfg->GetBaseElement(tEv.first)->GetBinIdx(), t_hits); 
      

      TH2Poly* h2Map = anodeCfg->ConstructPixHistMap(2, std::vector<int>{mtEv.first, tEv.first});

      for (const auto& pix : tEv.second->GetPixelEvents()) {
        double nhits = pix.second->GetNhits(); 
        if (nhits > hmax) hmax = nhits;
        h2Map->SetBinContent(pix.first, nhits); 
        if (nhits > 10000) 
          printf("Pixel %i (tile bin %i, MT %i) Collected %g e-\n", 
              pix.first, 
              mtCfg->GetBaseElement(tEv.first)->GetBinIdx(),
              mtEv.first, nhits);
      }

      pixMaps.push_back(h2Map); 
    }

    tileMaps.push_back(h2Tile); 
  }
  printf("hmax is %g\n", hmax);

  TH2Poly* h = anodeCfg->GetAnodeMap(0); 
  // Draw the tile-map
  TCanvas* cT = new TCanvas("cT"); 
  cT->cd(); h->DrawClone("axis"); 
  bool is_first = true; 
  for (auto &htile : tileMaps) {
    htile->GetZaxis()->SetRangeUser(0, htmax); 
    if (is_first) {htile->Draw("colz0 same"); is_first = false;}
    else htile->Draw("col0 same"); 
  }

  // Draw pixel hit-map

  TCanvas* cP = new TCanvas("cP"); 
  cP->cd(); h->DrawClone("axis"); 

  is_first = true; 
  for (size_t j=0; j<pixMaps.size(); j++) {
    auto qtile = pixMaps.at(j); 
    qtile->GetZaxis()->SetRangeUser(0, 1.05*hmax); 
    if (is_first) {qtile->Draw("colz0 same"); is_first = false;}
    else qtile->Draw("col0 same"); 
  }

  // Draw electron tracks
  auto pdgDB = TDatabasePDG::Instance();
  double q = 0;
  auto primaries = ev->GetPrimaries(); 
  for (const auto& pp : primaries) {
    auto trajectories = pp->GetTrajectories(); 
    for (const auto &tt : trajectories) {
      auto particle = pdgDB->GetParticle(tt->GetPDGID()); 
      if (particle) q = particle->Charge(); 
      else          q = 1; 
      if (tt->GetInitKineticEne() > 0.1 && q != 0) {
        TGraph* g = new TGraph(tt->GetPoints().size()); 
        int ip = 0; 
        for (const auto &tp : tt->GetPoints())  {
          g->SetPoint(ip, tp.fZ, tp.fY); ip++;
        }
        g->SetLineWidth(1); 
        if (tt->GetInitKineticEne() > 5.) {
          //g->SetMarkerStyle(20); 
          g->DrawClone("plsame");
        } else {
          g->DrawClone("lsame"); 
        }
      }
    }
  }

  

  return;
}

